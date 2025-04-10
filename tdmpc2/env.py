import subprocess
import numpy as np
import sim
import gym
from gym import spaces
import time
import torch
import threading
import psutil
from queue import Queue

class CoppeliaLilliEnvMultiprocess(gym.Env):
    def __init__(self, cfg):
        self.cfg = cfg
        super(CoppeliaLilliEnvMultiprocess, self).__init__()

        self.instances = []
        self.available_ports = Queue()

        for i in range(cfg.num_coppelia):   #setup instances
            port = 19997 + i
            scene_file = f"{cfg.scene_file}{i}.ttt"
            #print(f"[INIT] Starting instance {i} on port {port} with scene {scene_file}")
            process = self.start_coppelia_instance(cfg.coppelia_dir, scene_file)
            self.instances.append({"port": port, "process": process})
            self.available_ports.put(i)
            time.sleep(1)

        self.assign_instance()
        self.connect_to_coppelia()
        self.setup_simulation()

    def start_coppelia_instance(self, coppelia_dir, scene_file):
        command = ["xvfb-run", "-a", "./coppeliaSim.sh", "-h", "-s", scene_file]
        process = subprocess.Popen(
            command,
            cwd=coppelia_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        return process

    def kill_coppelia_instance(self, process):
        parent = psutil.Process(process.pid)
        #print(f"[KILL] Killing parent process PID: {parent.pid} {parent.name}")
        for child in parent.children(recursive=True):
            #print(f"[KILL] Killing child PID: {child.pid}")
            child.kill()
        parent.kill()

    def assign_instance(self):
        self.instance_id = self.available_ports.get()
        self.port = 19997 + self.instance_id
        self.process = self.instances[self.instance_id]["process"]
        #print(f"[ASSIGN] Using instance {self.instance_id} on port {self.port}")

    def connect_to_coppelia(self):
        sim.simxFinish(-1)
        self.client_id = sim.simxStart('127.0.0.1', self.port, True, True, 5000, 5)
        if self.client_id == -1:
            raise Exception(f"Failed to connect to CoppeliaSim on port {self.port}!")
        #print(f"[CONNECT] Connected to CoppeliaSim on port {self.port}")

    def setup_simulation(self):
        self.joint_names = [
            "/l_hip_x", "/r_hip_x", "/l_knee_x", "/r_knee_x",
            "/l_ankle_x", "/r_ankle_x", "/l_hip_z", "/r_hip_z", "/chest_z"
        ]
        self.jointHandles = []

        for joint in self.joint_names:
            res, joint_handle = sim.simxGetObjectHandle(self.client_id, joint, sim.simx_opmode_blocking)
            if res == sim.simx_return_ok:
                self.jointHandles.append(joint_handle)
            else:
                raise Exception(f"Failed to get handle for {joint}. Error code: {res}")

        res, self.pelvis_handle = sim.simxGetObjectHandle(self.client_id, "/pelvis_respondable",
                                                          sim.simx_opmode_blocking)

        self.action_space = spaces.Box(low=self.cfg.joints_lowest, high=self.cfg.joints_highest,
                                       shape=(len(self.joint_names),), dtype=np.float32)

    def _get_observation(self):
        joint_positions = []
        for joint in self.jointHandles:
            res, position = sim.simxGetJointPosition(self.client_id, joint, sim.simx_opmode_blocking)
            if res == sim.simx_return_ok:
                joint_positions.append(position)
            else:
                raise Exception(f"Failed to get joint position {joint}! - {res}")
        return torch.tensor(np.array(joint_positions, dtype=np.float32))

    def _compute_reward(self):
        # Get pelvis position
        res, pelvis_position = sim.simxGetObjectPosition(self.client_id, self.pelvis_handle, -1,
                                                         sim.simx_opmode_blocking)
        if res != sim.simx_return_ok:
            raise Exception("Failed to get pelvis position!")

        # Forward progress reward (Y-axis is forward in CoppeliaSim for your robot)
        forward_distance = -pelvis_position[1]
        delta_forward = forward_distance - getattr(self, "last_forward_position", 0.0)
        self.last_forward_position = forward_distance

        forward_reward = delta_forward * 10.0  # Reward for moving forward (tune multiplier)

        # Height penalty (fall detection)
        pelvis_height = pelvis_position[2]
        fall_penalty = 0.0
        if pelvis_height < 0.3:
            fall_penalty = -50.0  # Penalty for falling

        # Optional: Smoothness penalty (penalize large joint velocity or action)
        smoothness_penalty = 0.0
        if hasattr(self, "last_action"):
            action_diff = torch.norm(self.last_action - self.current_action)
            smoothness_penalty = action_diff.item() * 2.0  # Penalize jerky movements
        self.last_action = self.current_action.clone()

        # Optional: Energy penalty (penalize large joint angles)
        energy_penalty = torch.norm(self.current_action).item() * 0.1

        total_reward = forward_reward + fall_penalty - smoothness_penalty - energy_penalty
        return torch.tensor(total_reward, dtype=torch.float32)

    def _check_termination(self):
        res, pelvis_position = sim.simxGetObjectPosition(self.client_id, self.pelvis_handle, -1,
                                                         sim.simx_opmode_blocking)
        if res == sim.simx_return_ok:
            if self.current_step >= self.cfg.horizon and pelvis_position[2] < 0.3:
                #print("[TERMINATE] Lilli fell")
                return True, {"success": False}
            if -pelvis_position[1] > self.cfg.max_distance:
                print("[TERMINATE] Max distance exceeded")
                return True, {"success": True}
        if self.current_step >= self.cfg.episode_length:
            return True, {"success": True}
        return False, {"success": False}

    def rand_act(self):
        return torch.from_numpy(self.action_space.sample().astype(np.float32))

    def step(self, action):
        #print("[STEP]")
        obs = self._get_observation()
        next_obs = obs + action

        for i, target_angle in enumerate(next_obs):
            res = sim.simxSetJointTargetPosition(self.client_id, self.jointHandles[i], target_angle,
                                                 sim.simx_opmode_blocking)
            if res != sim.simx_return_ok:
                print(f"[STEP] Failed to set joint position for {self.joint_names[i]}!")

        time.sleep(0.05)
        sim.simxSynchronousTrigger(self.client_id)

        self.current_action = action.clone()
        reward = self._compute_reward()
        done, info = self._check_termination()

        self.current_step += 1
        return next_obs, reward, done, info

    def reset(self):
        old_instance_id = self.instance_id
        old_process = self.process

        def restart_old():
            #print(f"[THREAD] Restarting instance {old_instance_id} in background")
            self.kill_coppelia_instance(old_process)
            time.sleep(2)
            scene_file = f"{self.cfg.scene_file}{old_instance_id}.ttt"
            new_process = self.start_coppelia_instance(self.cfg.coppelia_dir, scene_file)
            self.instances[old_instance_id]["process"] = new_process
            #print(f"[THREAD] Instance {old_instance_id} restarted")

        self.available_ports.put(old_instance_id)
        self.assign_instance()

        try:
            self.connect_to_coppelia()
            self.setup_simulation()

            #print(f"[RESET] Switching from instance {old_instance_id} to {self.instance_id}")

            threading.Thread(target=restart_old, daemon=True).start()

            self.current_step = 0
            return self._get_observation()
        except:
            print("Crashed reset")
            return self.reset()

    def close(self):
        try:
            subprocess.run(["pkill", "-9", "coppeliaSim"], check=True)
            time.sleep(10)
            print("CoppeliaSim stopped!")
        except subprocess.CalledProcessError:
            print("CoppeliaSim was not running or could not be stopped.")