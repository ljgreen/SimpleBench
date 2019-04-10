Environment:

Linux 64bit

Prerequesites:

autoconf and automake are used to generate Makefile

Debian(e.g., Ubuntu, Debian): sudo apt-get install autoconf

RH(Redhat, CentOS, Fedora): sudo yum install autoconf 

INSTALL:

./configure

make

sudo make install

To uninstall Simplebench:

sudo make uninstall

Running:

sudo ./main -e [Duration] -f [Target file] -r [Range size] -s [Request size] -t [Type (R or W)] -p [Pattern (R or S)] -q [Threads (1 - 32)] -d [Direct IO (F or T)] -o [Trace file]

Running using script (Recommended):

Noting: you should guarantee you have the corresponding device /dev/sdd, otherwise you should make some changes

1. Duration:5 seconds, Device file: /dev/sdd, Range: 4096MB, Request size: 4KB, Type: read, Pattern: sequential, Queue depth:30, Direct IO: true, Trace file: /tmpfs/test.tr
./run1.sh (sudo ./main -e 5 -f /dev/sdd -r 4096 -s 4 -t R -p S -q 30 -d T -o /tmpfs/test.tr) 

2. Duration:5 seconds, Device file: /dev/sdd, Range: 4096MB, Request size: 4KB, Type: read, Pattern: random, Queue depth:30, Direct IO: true, Trace file: /tmpfs/test.tr
./run2.sh (sudo ./main -e 5 -f /dev/sdd -r 4096 -s 4 -t R -p R -q 30 -d T -o /tmpfs/test.tr) 

3. Duration:5 seconds, Device file: /dev/sdd, Range: 4096MB, Request size: 4KB, Type: write, Pattern: sequential, Queue depth:30, Direct IO: false, Trace file: /tmpfs/test.tr
./run3.sh (sudo ./main -e 5 -f /dev/sdd -r 4096 -s 4 -t W -p S -q 30 -d F -o /tmpfs/test.tr) 

4. Duration:5 seconds, Device file: /dev/sdd, Range: 4096MB, Request size: 4KB, Type: write, Pattern: random, Queue depth:30, Direct IO: false, Trace file: /tmpfs/test.tr
./run4.sh (sudo ./main -e 5 -f /dev/sdd -r 4096 -s 4 -t W -p R -q 30 -d F -o /tmpfs/test.tr) 

5. Duration:5 seconds, Regular file: /mnt/test.db, Range: 4096MB, Request size: 4KB, Type: read, Pattern: sequential, Queue depth:30, Direct IO: true, Trace file: /tmpfs/test.tr
./run5.sh (sudo ./main -e 5 -f /mnt/test.db -r 4096 -s 4 -t R -p S -q 30 -d T -o /tmpfs/test.tr) 

6. Duration:5 seconds, Regular file: /mnt/test.db, Range: 4096MB, Request size: 4KB, Type: read, Pattern: random, Queue depth:30, Direct IO: true, Trace file: /tmpfs/test.tr
./run6.sh (sudo ./main -e 5 -f /mnt/test.db -r 4096 -s 4 -t R -p R -q 30 -d T -o /tmpfs/test.tr) 

7. Duration:5 seconds, Regular file: /mnt/test.db, Range: 4096MB, Request size: 4KB, Type: write, Pattern: sequential, Queue depth:30, Direct IO: false, Trace file: /tmpfs/test.tr
./run7.sh (sudo ./main -e 5 -f /mnt/test.db -r 4096 -s 4 -t W -p S -q 30 -d F -o /tmpfs/test.tr) 

8. Duration:5 seconds, Regular file: /mnt/test.db, Range: 4096MB, Request size: 4KB, Type: write,Pattern: random, Queue depth:30, Direct IO: false, Trace file: /tmpfs/test.tr
./run8.sh (sudo ./main -e 5 -f /mnt/test.db -r 4096 -s 4 -t W -p R -q 30 -d F -o /tmpfs/test.tr) 

Author:

Jian Liu

Email: jian090032@gmail.com
