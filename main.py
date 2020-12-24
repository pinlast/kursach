import socket
import pprint
import seaborn as sns
import pandas as pd
from collections import defaultdict
import matplotlib.pyplot as plt


def listen():
    gathered_calls = []
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(('127.0.0.1', 8080))
        s.listen()
        conn, addr = s.accept()
        with conn:
            while True:
                data = conn.recv(2048).decode()
                if not data:
                    break
                gathered_calls.append(data.split(';'))
    return gathered_calls

def format_data(calls):
    counter = defaultdict(int)
    result = []
    for call in calls:
        counter[call[0]] += 1
    for key, value in counter.items():
        result.append({'syscall': key, 'кількість': value})
    return pd.DataFrame(result, columns=('syscall', 'кількість'))


def main():
    while True:
        try:
            print('listening')
            gathered_calls = listen()
            formatted = format_data(gathered_calls)
            sns.catplot(x="syscall", y='кількість', kind="bar", palette="ch:.25", data=formatted)
            plt.show()
        except KeyboardInterrupt:
            print('\nBye')
            break

if __name__ == '__main__':
    main()
