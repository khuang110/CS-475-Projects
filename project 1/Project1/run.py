import subprocess
import os
import multiprocessing
import csv

tests = [100, 10000, 100000, 1000000, 10000000]


def main(numt):
    num_tries = multiprocessing.cpu_count()
    fnull = open(os.devnull, 'w')
    data = []

    for j in range(1, num_tries+1):
        if j > 16:
            i = j-16
        else:
            i = j
        for k in range(numt):
            res = subprocess.Popen(["project1.exe", str(i), str(tests[k])], stdin=fnull, stdout=subprocess.PIPE,
                                   stderr=fnull, shell=False)
            if res.returncode == 1:
                exit(1)

            output = res.communicate()
            data.append([float(x) for x in output[0].split()])

    with open('project1.csv', mode='w', newline='') as f:
        w = csv.writer(f)
        w.writerow(["threads", "trials", "probability", "megatrails/sec"])
        for i in range(num_tries*numt):
            w.writerow(data[i])


if __name__ == "__main__":
    a = 5
    main(a)
