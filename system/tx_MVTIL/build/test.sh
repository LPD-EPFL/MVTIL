#!/bin/sh

parallel ./test_multi_client ::: xact1 xact2 xact3 xact4 xact5
echo all processes complete