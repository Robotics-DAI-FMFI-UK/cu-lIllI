import hydra
import torch

from termcolor import colored

from env import CoppeliaLilliEnvMultiprocess
from tdmpc2.common.logger import Logger
from tdmpc2.common.parser import parse_cfg
from tdmpc2.common.seed import set_seed
from tdmpc2.tdmpc2 import TDMPC2
from coppelia_lilli_env import CoppeliaLilliEnv
from tdmpc2.common.buffer import Buffer

from tdmpc2.trainer.offline_trainer import OfflineTrainer
from tdmpc2.trainer.online_trainer import OnlineTrainer


@hydra.main(config_name='config', config_path='.')
def train(cfg: dict):
    assert torch.cuda.is_available()
    assert cfg.steps > 0, 'Must train for at least 1 step.'
    cfg = parse_cfg(cfg)
    set_seed(cfg.seed)
    print(colored('Work dir:', 'yellow', attrs=['bold']), cfg.work_dir)
    trainer_cls = OfflineTrainer if cfg.multitask else OnlineTrainer
    if cfg.multiprocess:
        env = CoppeliaLilliEnvMultiprocess(cfg)
    else:
        env = CoppeliaLilliEnv(cfg)
    trainer = trainer_cls(
        cfg=cfg,
        env=env,
        agent=TDMPC2(cfg),
        buffer=Buffer(cfg),
        logger=Logger(cfg),
    )
    trainer.train()
    print('\nTraining completed successfully')
    env.close()


if __name__ == "__main__":
    train()
