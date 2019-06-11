#!/bin/bash

if [ $# -lt 2 ]; then
        echo "x---x---x---x---x---x---x---x---x"
        echo "No command line argument supplied"
        echo "Run again with jobscript folder and data folder name  as cmd line inputs"
	      echo "Usage: ./be-genjob.sh <job_folder> <data_folder>"
        echo "x---x---x---x---x---x---x---x---x"
        exit 0
fi

#bone-be parameters
TIMESTEP=1
#E_SIZE="5 6 9 11 16 17" 
#EPP="4,4,4"
#NP="16 256 2048 16384 131072" 
#NP="131072" 
PHY_PARAM=5

#Test case!!
E_SIZE="9"
EPP="4,4,4" #"8 24 70 125 180"
NP="524288" #"2048 16384" #"131072"
#CART_X=32
#CART_Y=16
#CART_Z=16

#Machine Parameters
Tlnum_proc=16
one=1

#Creating array stack for EPP
epp_stack=$(echo $EPP | tr ";" "\n")

#Check for job dir to store job scripts
if [ ! -d "$1" ]; then
    mkdir $1/
fi

#Check for data dir to store *.out and *.err
if [ ! -d "$2" ]; then
    mkdir $2/
    mkdir $2/err
fi

echo "Creating job file(s)..."

for ELE_SIZE in $E_SIZE
do
  for Exyz in $epp_stack
  do
    #Calculating EL_X, EL_Y,EL_Z from epp_stack
    EL_X=$(echo $Exyz | tr "," " " | awk '{print $1}')
    EL_Y=$(echo $Exyz | tr "," " " | awk '{print $2}')
    EL_Z=$(echo $Exyz | tr "," " " | awk '{print $3}')
    EC=$((EL_X*EL_Y*EL_Z))
    for NUM_TASKS in $NP
    do
      #Time and memory assignment
      if [ $NUM_TASKS -eq 131072 ];
        then
        MEM="1800mb"
        TASKS=256
        if [ $EC -le 100 ]; then
          wtime="2-05:30:00"
        elif [ $EC -ge 128 ]; then
          wtime="2-06:30:00"
        else
          wtime="2-07:00:00"
        fi
      elif [ $NUM_TASKS -eq 16384 ];
        then
        MEM="600mb"
        TASKS=32
        if [ $EC -le 100 ]; then
          wtime="01:30:00"
        elif [ $EC -ge 128 ]; then
          wtime="01:55:00"
        else
          wtime="02:15:00"
        fi
      elif [ $NUM_TASKS -eq 2048 ]; 
        then
        MEM="512mb"
        TASKS=16
        if [ $EC -le 100 ]; then
          wtime="00:21:00"
        elif [ $EC -ge 128 ]; then
          wtime="00:35:00"
        else
          wtime="00:50:00"
        fi
      elif [ $NUM_TASKS -eq 256 ]; 
        then
        MEM="400mb"
        TASKS=4
        if [ $EC -le 100 ]; then
          wtime="00:15:00"
        elif [ $EC -eq 128 ]; then
          wtime="00:20:00"
        elif [ $EC -eq 256 ]; then
          wtime="00:30:00"
        else
          wtime="00:40:00"
        fi
      elif [ $NUM_TASKS -eq 16 ]; 
        then
        MEM="256mb"
        TASKS=1
        if [ $EC -le 100 ]; then
          wtime="00:10:00"
        elif [ $EC -ge 128 ]; then
          wtime="00:15:00"
        else
          wtime="00:30:00"
        fi
      elif [ $NUM_TASKS -eq 524288 ];
	then
	MEM="50gb"
	wtime="1:00:00"
	TASKS=128
      else
	MEM="60gb"
	wtime="4:00:00"
	TASKS=128
      fi

      NUM_NODES1=$((TASKS/Tlnum_proc))
      NUM_NODESr=$((TASKS%Tlnum_proc))
      if [ $NUM_NODESr -eq 0 ];
      then
          NUM_NODES=$NUM_NODES1
      else
          NUM_NODES=$((NUM_NODES1+one))
      fi

#  	  if [ $EL_X = $EL_Y ] && [ $EL_Y = $EL_Z ]; 
#  	  then
      	  #Making the job script
      	  echo '#!/bin/bash' > jobfile
      	  echo '#MSUB -d '`pwd` >> jobfile
      	  echo '#MSUB -N be_sim-es'$ELE_SIZE'ec'$EC'np'$NUM_TASKS'.job' >>jobfile	#Job name
      	  echo "#MSUB -l nodes=$NUM_NODES" >> jobfile		#Number of Nodes
      	  echo "#MSUB -l partition=cab" >> jobfile			#Per processor memory
          echo '#MSUB -q pbatch' >> jobfile  #task per socket
      	  echo "#MSUB -l walltime=$wtime" >> jobfile				#Walltime
      	  echo '#MSUB -o '$2'/bonebe-es'$ELE_SIZE'ec'$EC'np'$NUM_TASKS'.csv' >> jobfile	#STDOUT
      	  echo '#MSUB -e '$2'/err/be_sim-es'$ELE_SIZE'ec'$EC'np'$NUM_TASKS'.err' >> jobfile	#STDERR
      	  echo " " >> jobfile
      	  echo "module load gnu/4.9.2 openmpi-gnu/1.8" >> jobfile
          echo 'export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/lib32"' >> jobfile
          echo 'export BOOST_HOME=$HOME/local/packages/boost-1.56' >> jobfile
          echo 'export LD_LIBRARY_PATH=$BOOST_HOME/lib:$LD_LIBRARY_PATH' >> jobfile
          echo 'export SST_CORE_HOME=$HOME/local/sst-core-6.0' >> jobfile
          echo 'export PATH=$SST_CORE_HOME/bin:$PATH' >> jobfile
          echo 'export SST_ELEMENTS_HOME=$HOME/local/sst-elements-6.0' >> jobfile
          echo 'export PATH=$SST_ELEMENTS_HOME/bin:$PATH' >> jobfile
      	  echo " " >> jobfile
      	  echo './BE_RUN.sh -c vulcan-proper-es'$ELE_SIZE'ec'$EC'np'$NUM_TASKS'.py -n '$TASKS >> jobfile
      	  mv jobfile be_sim'-es'$ELE_SIZE'ec'$EC'np'$NUM_TASKS'.job'
          mv be_sim'-es'$ELE_SIZE'ec'$EC'np'$NUM_TASKS'.job' $1
#  	  fi
    done
  done
done

echo "Job file(s) created!"
echo " "
echo "Listing Jobfile(s):"
echo "---------------------------------------------------------------"
ls $1
echo "----------------------------------------------------------------"
sleep 2
echo " "
echo "Job file(s) present in $1/ folder"
echo "Output and error files present in $2/ folder"
echo " "

#Submit jobscript
echo "Submitting Job files:-"

for ELE_SIZE in $E_SIZE
do
 for Exyz in $epp_stack
 do
  EL_X=$(echo $Exyz | tr "," " " | awk '{print $1}')
  EL_Y=$(echo $Exyz | tr "," " " | awk '{print $2}')
  EL_Z=$(echo $Exyz | tr "," " " | awk '{print $3}')
  EC=$((EL_X*EL_Y*EL_Z))
  for NUM_TASKS in $NP
  do
#   if [ $EL_X = $EL_Y ] && [ $EL_Y = $EL_Z ];
#   then
      msub $1/be_sim'-es'$ELE_SIZE'ec'$EC'np'$NUM_TASKS'.job'
#   fi
  done
 done
done

echo "* * * * -----------Completed!----------- * * * *"
