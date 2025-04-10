import subprocess

import numpy as np
import sim
import gym
from gym import spaces
import time
import torch


class CoppeliaLilliEnv(gym.Env):
    def __init__(self, cfg):
        self.cfg = cfg
        super(CoppeliaLilliEnv, self).__init__()
        self.reconnect()

    def reconnect(self):
        if not self.cfg.manual_evaluation:
            self.stop_coppelia()
            self.start_coppelia()
            print("CoppeliaSim started!")
        sim.simxFinish(-1)
        self.client_id = sim.simxStart('127.0.0.1', 19997, True, True, 5000, 5)
        if self.client_id == -1:
            raise Exception("Failed to connect to CoppeliaSim!")
        print("Connected to CoppeliaSim!")

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
        # self.observation_space = spaces.Box(low=-np.pi, high=np.pi, shape=(len(self.joint_names),), dtype=np.float32)

    def start_coppelia(self):
        coppelia_dir = self.cfg.coppelia_dir
        scene_file = self.cfg.scene_file
        command = ["xvfb-run", "-a", "./coppeliaSim.sh", "-h", scene_file]

        process = subprocess.Popen(
            command,
            cwd=coppelia_dir,  # Set working directory to CoppeliaSim folder
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        time.sleep(10)
        print("CoppeliaSim started!")
        return process

    def stop_coppelia(self):
        try:
            subprocess.run(["pkill", "-9", "coppeliaSim"], check=True)
            time.sleep(10)
            print("CoppeliaSim stopped!")
        except subprocess.CalledProcessError:
            print("CoppeliaSim was not running or could not be stopped.")

    def _get_observation(self):
        """Retrieve joint angles as the state."""
        joint_positions = []
        for joint in self.jointHandles:
            res, position = sim.simxGetJointPosition(self.client_id, joint, sim.simx_opmode_blocking)
            if res == sim.simx_return_ok:
                joint_positions.append(position)
            else:
                print(f"Failed to get joint position! - {res}")
                raise Exception(f"Failed to get joint position! - {res}")
        obs_array = np.array(joint_positions, dtype=np.float32)
        return torch.tensor(obs_array)

    def _compute_reward(self):
        """Calculate reward based on how far the robot moved forward."""
        res, pelvis_position = sim.simxGetObjectPosition(self.client_id, self.pelvis_handle, -1,
                                                         sim.simx_opmode_blocking)
        if res == sim.simx_return_ok:
            reward = -pelvis_position[1]  # Lower y-value = better progress
            if reward > self.cfg.max_distance:
                reward += self.cfg.episode_length - self.current_step
            return torch.tensor(reward, dtype=torch.float32)  # Convert to PyTorch tensor
        else:
            raise Exception("Failed to get pelvis position!")

    def _compute_reward2(self):
        """Reward is higher when the robot's legs (hips) are further apart in x-axis."""
        res_l, l_hip_pos = sim.simxGetObjectPosition(self.client_id,
                                                     self.jointHandles[self.joint_names.index("/l_hip_x")],
                                                     -1, sim.simx_opmode_blocking)
        res_r, r_hip_pos = sim.simxGetObjectPosition(self.client_id,
                                                     self.jointHandles[self.joint_names.index("/r_hip_x")],
                                                     -1, sim.simx_opmode_blocking)

        if res_l == sim.simx_return_ok and res_r == sim.simx_return_ok:
            # Calculate Euclidean distance (or just x-distance)
            x_dist = abs(l_hip_pos[0] - r_hip_pos[0])
            reward = torch.tensor(x_dist, dtype=torch.float32)
            return reward
        else:
            raise Exception("Failed to get hip positions for reward calculation!")

    def _check_termination(self):
        """End the episode if the robot falls or after 50 steps."""
        res, pelvis_position = sim.simxGetObjectPosition(self.client_id, self.pelvis_handle, -1,
                                                         sim.simx_opmode_blocking)
        if res == sim.simx_return_ok:
            if self.current_step >= self.cfg.horizon and pelvis_position[
                2] < 0.3:  # If pelvis z-position < 0.3, robot has fallen
                return True, {"success": False}
            if -pelvis_position[1] > self.cfg.max_distance:
                print("More than 2!")
                return True, {"success": True}
        else:
            raise Exception("Failed to get pelvis position!")

        # Stop after 50 steps
        if self.current_step >= self.cfg.episode_length:
            return True, {"success": True}

        return False, {"success": False}

    def rand_act(self):
        return torch.from_numpy(self.action_space.sample().astype(np.float32))

    def _obs_to_tensor(self, obs):
        raise Exception(f"Called _obs_to_tensor, not implemented...")

    def step(self, action):
        """Execute action, step simulation, and return new state, reward, done flag."""

        obs = self._get_observation()
        next_obs = obs + action

        for i, target_angle in enumerate(next_obs):
            res = sim.simxSetJointTargetPosition(self.client_id, self.jointHandles[i], target_angle,
                                                 sim.simx_opmode_blocking)
            if res != sim.simx_return_ok:
                print(f"Failed to set joint position for {self.joint_names[i]}!")
                # raise Exception(f"Failed to set joint position for {self.joint_names[i]}!")

        # Step simulation
        time.sleep(0.05)  # Give the simulator some time
        sim.simxSynchronousTrigger(self.client_id)

        # Compute reward
        reward = self._compute_reward2()  # tuto treba prepisat!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        # Check if episode is done
        done, info = self._check_termination()

        self.current_step += 1
        return next_obs, reward, done, info

    def reset(self):
        """Reset simulation and return initial state."""
        sim.simxStopSimulation(self.client_id, sim.simx_opmode_blocking)
        time.sleep(1)  # Wait for the simulation to stop

        res = sim.simxStartSimulation(self.client_id, sim.simx_opmode_blocking)

        if res != sim.simx_return_ok:
            print(f"Failed to start simulation! Error code: {res}")

        self.current_step = 0  # Reset step counter
        try:
            return self._get_observation()
        except:
            self.reconnect()
            print("Restarted CoppeliaSim")
            return self._get_observation()

    def close(self):
        try:
            subprocess.run(["pkill", "-9", "coppeliaSim"], check=True)
            time.sleep(10)
            print("CoppeliaSim stopped!")
        except subprocess.CalledProcessError:
            print("CoppeliaSim was not running or could not be stopped.")
