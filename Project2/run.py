import subprocess
import os, csv
import multiprocessing as mp

# Num nodes
tests = [100, 500, 1000, 2500, 5000, 7500, 10000, 25000, 100000]
tesats = [5000]

def main(*args):
    numt = _numt if args[0] > 0 else mp.cpu_count()
    num_tries = len(tests)
    fnull = open(os.devnull, 'w')
    y = ['']
    data = {}

    path, f_name = get_path(file + '.exe')
    if f_name is None:
        raise FileNotFoundError("File not found!!")

    for j in range(1, numt+1):
        print("Thread %d working:"% j)
        for k in range(num_tries):
            print("%s "% tests[k], end='')
            res = subprocess.Popen([f_name, str(j), str(tests[k])], stdin=fnull, stdout=subprocess.PIPE,
                                   stderr=fnull, shell=False)
            if res.returncode == 1:
                raise ChildProcessError("Executable failed!!!")

            output = res.communicate()
            vals = [float(x) for x in output[0].split()]
            if vals[1] not in y:
                y.append(vals[1])
            data[(vals[0], vals[1])] = vals[2]
        print()
    csv_file = ""

    for i in range(numt):
        row = [str(i+1)]
        for j in tests:
            curr_pair = (i+1, j)
            row.append("%s"% (data[curr_pair]))
        print(row)
        csv_file += "%s\n" % ",".join(row)

    with open(file + '.csv', mode='w', newline='') as f:
        w = csv.writer(f)
        w.writerow(['', 'Num Nodes'])
        w.writerow(['Threads'])
        w.writerow(y)
        f.write(csv_file)


def get_path(file_name):
    """ This function searches for a given file name in current and subdirectories
        and returns the path.
    :param:
        file_name (string): Name of file to search for
    :return:
        string, string: file path, file path with file appended
    """
    root_dir = os.path.dirname(__file__)

    for sub_dir, dirs, files in os.walk(root_dir):
        for file in files:
            if file == file_name:
                return sub_dir, os.path.join(sub_dir, file)
    return None, None


if __name__ == "__main__":
    _numt = 0
    file = 'Project2'
    main(_numt, file)
