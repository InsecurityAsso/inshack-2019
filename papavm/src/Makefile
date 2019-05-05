all: upx

upx: full
	upx -o ../public-files/papavm full
	rm -rf full exec vm troll vm_code.h vm_split.h vm_troll.h exec1 exec2
	sed -i 's/UPX/ZOB/g' ../public-files/papavm

full: split full.c
	gcc full.c -o full -static
	strip -s full

split: exec
	python split.py exec exec1 exec2 > vm_split.h

exec: packed exec.c
	gcc exec.c -o exec
	strip -s exec

packed: vm
	python encrypt.py > vm_code.h

vm: troll vm.c
	gcc -e justforfun_real_main vm.c vm_dir/core_vm.c -o vm
	strip -s vm -K main

troll: troll.rs
	rustc troll.rs
	python troll.py troll > vm_troll.h

clean:
	rm -rf exec exec1 exec2 exec_unsplit vm_code.h vm vm2 full final vm_split.h vm_troll.h troll