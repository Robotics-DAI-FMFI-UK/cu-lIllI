import sim
import torch


def _compute_reward(self):
    res, pelvis_position = sim.simxGetObjectPosition(self.client_id, self.pelvis_handle, -1,
                                                     sim.simx_opmode_blocking)
    if res == sim.simx_return_ok:
        reward = -pelvis_position[1]
        if reward > self.cfg.max_distance:
            reward += self.cfg.episode_length - self.current_step
        return torch.tensor(reward, dtype=torch.float32)
    else:
        raise Exception("Failed to get pelvis position!")


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



def _compute_reward2(self): # pouzite na primitivne RL aby robot spravil snuru
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


