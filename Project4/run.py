import subprocess
import os, csv
import multiprocessing as mp
#from tqdm import tqdm

# Num nodes
tests = [100, 500, 1000, 2500, 5000, 7500, 10000, 25000, 100000]


def main(*args):
    numt = _numt if args[0] > 0 else mp.cpu_count()
    num_tries = len(tests)
    fnull = open(os.devnull, 'w')
    y = []
    data = []

    # path, f_name = get_path(file + '.exe')
    # if f_name is None:
    #     raise FileNotFoundError("File not found!!")
    print("Running Simulation...")
    for t in (range(1, numt + 1, 2)):
        for k in range(10, 24):
            res = subprocess.Popen([file, str(k), str(t)], stdin=fnull, stdout=subprocess.PIPE,
                                   stderr=fnull, shell=False)
            if res.returncode == 1:
                raise ChildProcessError("Executable failed!!!")

            output = res.communicate()
            vals = [float(x) for x in output[0].split()]
            #print(vals)
            #print("---------------")
            data.append(vals)

    csv_file = ""
    print("len : ",len(data))
    print("Sending to csv...")
    for i in (range(14 * numt)):
        row = [str((i%14)+10)]
        for j in range(2):
            row.append("%s"% (data[i][j]))
        #print(row)
        csv_file += "%s\n" % ",".join(row)

    with open('Project4' + '.csv', mode='w', newline='') as f:
        w = csv.writer(f)
        f.write(csv_file)
    print("Done")


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
    _numt = 1
    file = './arraymult'
    main(_numt, file)
