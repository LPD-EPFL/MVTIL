from fabric.api import local, run, env, execute, settings
from fabric.context_managers import shell_env

from collections import defaultdict
import subprocess
import time
import os,sys

server_port = 6060
key_space = 1000
#Test cases
#Server cluster list
s_hosts = ['lpdquad.epfl.ch','lpdxeon2680.epfl.ch','lpd48core.epfl.ch']
#Client cluster list
c_hosts = ['lpdquad.epfl.ch','lpdxeon2680.epfl.ch','lpd48core.epfl.ch']
#algorithm's directory
alg_dir = "mvtil_ex/"

def connect(hosts):
	env.hosts = hosts
	env.user = 'xxx'
	env.password = 'xxx'
	env.parallel = True

def server_start(alg,param):
	connect(s_hosts)
	execute(server_run,alg,param)

#Invoke server start command
def server_run(alg,param):
	command = alg_dir + 'tx_'+alg+'/build/server_exec -p'+str(server_port)+" "+param
	with shell_env(LD_LIBRARY_PATH="/home/jwang/lib:/usr/lib:/usr/local/lib"):
		with settings(warn_only=True):
			run('kill $(lsof -i:%d -t) > /dev/null'%server_port)
		run(command)

def client_start(alg,test_id,c_threads,tx_info):
	connect(c_hosts)
	output = execute(client_run,alg,test_id,c_threads)
	commit_tx = sum(int(res[0]) for ip,res in output.items())
	total_tx = sum(int(res[1]) for ip,res in output.items())
	tx_info.append(commit_tx)
	tx_info.append(total_tx)

#Invoke client start command
def client_run(alg,test_id,c_threads):
	cid = c_hosts.index(env.host)
	with shell_env(LD_LIBRARY_PATH="/home/jwang/lib:/usr/lib:/usr/local/lib"):
		command = alg_dir + 'tx_'+alg+'/build/performance_single -l20000 -t'+str(test_id)+" -c"+ str(c_threads) +" -i"+ str(cid) + " -k"+str(key_space)
		output = run(command).split()
		return output

def execute_server_command(alg,param):
	command = 'fab -f run_local_cluster.py server_start:alg=%s,param=%s'%(alg,param)
	subprocess.Popen(command, shell=True)

def execute_client_command(alg,test_id,c_threads):
	tx_info = []
	client_start(alg,test_id,c_threads,tx_info)
	return tx_info[0],tx_info[1]

def kill_server(processes):
	for p in processes:
		print("kill")
		p.terminate()
		p.wait()
	time.sleep(10)

def test_all():
	f = open('rec_dis_short.dat', 'w')
	algs =["2PL","MVTO+","MVTIL","MVTIL"]
	server_param = ["-t10","-t10","-Pf -t10","-Pl -t10"]
	client_threads = [2,5,10,20,30,40,50,60,80,100,120,140,160,180,200]
	test_cases = {0:"ReadOnly",1:"ReadIntensive",2:"WriteIntensive",3:"RW",4:"Short",5:"Mix"}
	try:
		for test_id,test_name in test_cases.items():
			f.write("#Test_case: "+ test_name + "\n")
			for num in range(len(algs)):
				test_alg = algs[num]
				test_param = server_param[num]
				#Server start
				execute_server_command(test_alg,test_param)
				time.sleep(20)
				f.write("#Algorithm: "+ test_alg +"\n")
				for t_num in client_threads:
					#Client start
					commit_tx,total_tx = execute_client_command(test_alg,test_id,t_num)
					f.write(str(t_num)+" "+str(commit_tx)+" "+str(total_tx)+'\n')
					f.flush()
				f.write("\n\n")	
		f.close()
	except subprocess.CalledProcessError:
		kill_server(processes)

if __name__ == '__main__':
	test_all()
