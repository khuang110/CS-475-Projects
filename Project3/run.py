import subprocess
import os, csv
import multiprocessing as mp
from datetime import datetime as dt

# Num nodes
tests = [100, 500, 1000, 2500, 5000, 7500, 10000, 25000, 100000]
tesats = [5000]

def main(*args):
    numt = _numt
    num_tries = len(tests)
    fnull = open(os.devnull, 'w')
    y = ['']
    data = []

    # path, f_name = get_path(file)
    # if f_name is None:
    #     raise FileNotFoundError("File not found!!")

    res = subprocess.Popen([file, ], stdin=fnull, stdout=subprocess.PIPE,
                           stderr=fnull, shell=False)
    if res.returncode == 1:
        raise ChildProcessError("Executable failed!!!")

    output = res.communicate()
    vals = [x for x in output[0].split()]

    if numt == 3:
        for i in range(0, len(vals)-5, 5):
            date = dt.strptime(vals[i].strip().decode(), '%m-%Y').date()
            data.append([str(date.strftime('%Y-%m').strip('(')), float(vals[i+1]), float(vals[i+2]), float(vals[i+3]), float(vals[i+4])])
    else:
        for i in range(0, len(vals)-6, 6):
            date = dt.strptime(vals[i].strip().decode(), '%m-%Y').date()
            data.append([str(date.strftime('%Y-%m').strip('(')), float(vals[i+1]), float(vals[i+2]), float(vals[i+3]), float(vals[i+4]), float(vals[i+5])])

    csv_file = ""

    for i in range(len(data)):
        row = []
        for j in range(numt+2):
            row.append("%s" % str(data[i][j]))
        print(row)
        csv_file += "%s\n" % ",".join(row)

    with open(file + '.csv', mode='w', newline='') as f:
        w = csv.writer(f)
        if numt == 3:
            w.writerow(['Date', 'Precip (cm)', 'Temp (F)', 'Height (cm)', 'Deer'])
        else:
            w.writerow(['Date', 'Precip (cm)', 'Temp (F)', 'Height (cm)', 'Deer', 'Relative Humidity (%)'])
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
    _numt = 4
    file = './proj3'
    main(_numt, file)
