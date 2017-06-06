import urllib.request
import requests
import sys
import os
import shutil
import subprocess


class helpers(object):
    """Utility class"""

    def download(link, file_name):
        if os.path.exists(file_name):
            print ('Warning: Archive file {} already present, skipping download'.format(file_name))
            return True

        with open(file_name, "wb") as f:
            print ("Downloading %s" % file_name)
            response = requests.get(link, stream=True)
            total_length = response.headers.get('content-length')

            if total_length is None: # no content length header
                f.write(response.content)
            else:
                dl = 0
                total_length = int(total_length)
                for data in response.iter_content(chunk_size=4096):
                    dl += len(data)
                    f.write(data)
                    done = int(50 * dl / total_length)
                    sys.stdout.write("\r[%s%s]" % ('=' * done, ' ' * (50-done)) )    
                    sys.stdout.flush()
                sys.stdout.write("\n")
                sys.stdout.flush()
        return True

    def copy(src, dst):
        for i in src:
            shutil.copy(i, dst)

    def move(src, dst):
        for i in src:
            try:
                dstf = dst;
                if os.path.isdir(dstf):
                    dstf = os.path.join(dst, os.path.basename(i))
                if os.path.exists(dstf):
                    os.unlink(dstf)
                shutil.move(i, dstf)
            except Exception as ex:
                print ('Warning, cannot copy {}: {}'.format(src, ex))

    def mkdir(dir):
        if not os.path.exists(dir):
            os.mkdir(dir)

    def execute(cmdline, outfile, errfile):
        out = open(outfile, "w")
        err = open(errfile, "w")
        
        print(cmdline, file=out)
        process = subprocess.Popen(cmdline, shell=True, stderr=err, stdout=subprocess.PIPE) #stdout=out, 
        for line in process.stdout:
            sys.stdout.buffer.write(line)
            sys.stdout.flush()
            out.buffer.write(line)
        process.wait()

        return process.returncode == 0
