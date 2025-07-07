#!/usr/bin/env python3
import sys
import argparse
from ai.models import GameConfig
from ai.zappy_ai import ZappyAI

def main():
    parser = argparse.ArgumentParser(prog='zappy_ai', description='Zappy AI Client', add_help=False)
    parser.add_argument('-p', type=int, required=True, help='port number')
    parser.add_argument('-n', type=str, required=True, help='name of the team')
    parser.add_argument('-h', type=str, default='localhost', dest='machine', help='name of the machine')
    try:
        args = parser.parse_args()
        config = GameConfig(host=args.machine, port=args.p, team_name=args.n)
        ai = ZappyAI(config)
        if ai.connect():
            ai.run_intelligent_strategy()
        else:
            sys.exit(1)
    except KeyboardInterrupt:
        print("\nAI interrupted by user")
        sys.exit(0)
    except Exception:
        sys.exit(1)

if __name__ == "__main__":
    main()
