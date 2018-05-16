#Fix the number of server while increasing the number of client
from fabric.api import local, env, execute, settings
from fabric.operations import run, put
from fabric.context_managers import shell_env

from collections import defaultdict
import xml.etree.cElementTree as ET
import subprocess
import time
import os,sys
import boto3

server_port = 6060
key_space = 10000
#Test cases
ec2 = boto3.resource('ec2')
running_instances = ec2.instances.filter(Filters=[{
	'Name': 'instance-state-name',
	'Values': ['running']}])

running_instances_ip = [instance.private_ip_address for instance in running_instances]

#The number of server
s_num = 8
#server lists
s_hosts = running_instances_ip[:s_num]
#client lists
c_hosts = running_instances_ip

alg_dir = "system/"

def connect(hosts):
	env.hosts = hosts
	env.user = 'ec2-user'
	env.parallel = True

def server_start(alg,param):
	connect(s_hosts)
	execute(server_run,alg,param)

def server_run(alg,param):
	command = '%stx_%s/build/server_exec -p%d %s'%(alg_dir,alg,server_port,param)
	with shell_env(LD_LIBRARY_PATH="usr/lib:/usr/local/lib"):
		with settings(warn_only=True):
			run('kill $(lsof -i:%d -t) > /dev/null'%server_port)
		run(command)

def generate_config_file():
	root = ET.Element("DataServers")
	for info in s_hosts:
		print(info)
		doc = ET.SubElement(root, "DataServer")
		ET.SubElement(doc, "ip").text = info
		ET.SubElement(doc, "port").text = str(server_port)
	with open("server_configure.xml", 'w') as f:
		f.write(ET.tostring(root, encoding='utf8'))
		f.flush()

def put_file():
	put("server_configure.xml","/home/ec2-user/servers.xml")
	run("cp system/tx_2PL/build/oracle.xml oracle.xml")

def send_config_file():
	generate_config_file()
	connect(c_hosts)
	execute(put_file)

def client_start(hosts,alg,test_id,c_threads,tx_info):
	connect(hosts)
	output = execute(client_run,alg,test_id,c_threads)
	commit_tx = sum(int(res[0]) for ip,res in output.items())
	total_tx = sum(int(res[1]) for ip,res in output.items())
	tx_info.append(commit_tx)
	tx_info.append(total_tx)

def client_run(alg,test_id,c_threads):
	cid = c_hosts.index(env.host)
	with shell_env(LD_LIBRARY_PATH="/usr/lib:/usr/local/lib"):
		command = '%stx_%s/build/performance -l20000 -t%d -c%d -i%d -k%d'%(alg_dir,alg,test_id,c_threads,cid,key_space)
		output = run(command).split()
		return output

def invoke_server_command(alg,param):
	command = 'fab -f run_ec2_client_scale.py server_start:alg=%s,param=%s'%(alg,param)
	subprocess.Popen(command, shell=True)

def invoke_client_command(hosts,alg,test_id,c_threads):
	tx_info = []
	client_start(hosts,alg,test_id,c_threads,tx_info)
	return tx_info[0],tx_info[1]

def kill_server(processes):
	for p in processes:
		print("kill")
		p.terminate()
		p.wait()
	time.sleep(10)

def test_all():
	send_config_file()
	f = open('rec.dat', 'w')
	#test algorithm
	algs = ["MVTO+","MVTIL","2PL","MVTIL"]
	#test parameter
	server_param = ["-t10","-Pf -t10","-t10","-Pl -t10"]
	test_cases = {0:"ReadOnly",1:"ReadIntensive",2:"WriteIntensive",3:"RW",4:"Short",5:"Mix"}
	client_threads = 1
	client_nums = range(5,400,15)
	try:
	    for test_id,test_name in test_cases.items():
		    f.write("#Test_case: "+ test_name + "\n")
		    for num in range(len(algs)):
			    test_alg = algs[num]
			    test_param = server_param[num]
			    f.write("#Algorithm: "+ test_alg +"\n")
			    for c_num in client_nums:
				    c_hosts = running_instances_ip[s_num:s_num+c_num]
				    #invoke server command
				    invoke_server_command(test_alg,test_param)
				    time.sleep(20)
				    #invoke client command
				    commit_tx,total_tx = invoke_client_command(c_hosts,test_alg,test_id,client_threads)
				    f.write('%d %d %d\n'%(c_num,commit_tx,total_tx))
				    f.flush()
			    f.write("\n\n")
	    f.close()
	except subprocess.CalledProcessError:
		kill_server(processes)

if __name__ == '__main__':
	test_all()