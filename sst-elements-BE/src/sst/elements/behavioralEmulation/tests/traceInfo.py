import ast

if __name__ == "__main__":

	with open("event_trace.txt", 'r') as fin:
        	content = fin.readlines()
        
        traceDict = {}
        commTrace = {}
	#event_list = [ast.literal_eval(x.strip('\n')) for x in content]
        for record_s in content:

                record = ast.literal_eval(record_s.strip('\n'))
                gid = record[0]

		if record[1] == "call":

			if gid in traceDict: traceDict[gid].append(record)
 
			else: traceDict.update({gid: [record]})

		if record[1] == "comm":

			locations = record[4][0][4]

			if len(locations) == 0:

				if gid in traceDict: traceDict[gid].append(record)
	 
				else: traceDict.update({gid: [record]})

                        
			if gid in commTrace:
				
				unique_id = (record[2][0], record[2][1])
                                rid = [record[3][0], record[3][1]]

				if unique_id in commTrace[gid]:

					if len(locations) == 0: commTrace[gid][unique_id][0] = (rid, record[4][0])

					else: commTrace[gid][unique_id][1].append((record[4][0], locations.index(record[4][0][0])))

				else:

					if len(locations) == 0: commTrace[gid].update({unique_id: [(rid, record[4][0]), []]})

					else: commTrace[gid].update({unique_id: [(), [(record[4][0], locations.index(record[4][0][0]))]]})

			else:
	
				unique_id = (record[2][0], record[2][1])
                                rid = [record[3][0], record[3][1]]

				if len(locations) == 0: commTrace.update({gid: {unique_id: [(rid, record[4][0]), []]}})

				else: commTrace.update({gid: {unique_id: [(), [(record[4][0], locations.index(record[4][0][0]))]]}})


	for gid in commTrace:

		for uniqueId in commTrace[gid]:

			messagePath = len(commTrace[gid][uniqueId][1][0][0][4])*[None]
                        messagePath[0] = (commTrace[gid][uniqueId][0][1][0], commTrace[gid][uniqueId][0][1][1], commTrace[gid][uniqueId][0][1][2], commTrace[gid][uniqueId][0][1][3])
         
                        record = (gid, "comm", uniqueId, commTrace[gid][uniqueId][0][0], messagePath) 
                        
			for path in commTrace[gid][uniqueId][1]:
				record[4][path[1]] = (path[0][0], path[0][1], path[0][2], path[0][3])

			pos = 0
			for o_record in traceDict[gid]:

				if o_record[2] == record[2]:
					traceDict[gid][pos] = record
					break

				pos = pos + 1


	prev = []
        for gid in traceDict:

        	for record in traceDict[gid]:

	        	if record[1] == "comm":

                                if record[-1][-1][0] in traceDict:

    	        			for recv_record in traceDict[record[-1][-1][0]]:

   	                        		if recv_record[3] == "wait" and recv_record[-1][1] == record[2][0] and recv_record[2] not in prev: 
                                
							record[3][-1] = recv_record[2][-1]
                                			prev.append(recv_record[2]) 
                                			break
         

	with open("trace_op", 'w') as fout:

        	for gid in traceDict:

                        pos = 0
                	for record in traceDict[gid]:

				if record[1] == "call":

					record_mod = (record[1], tuple(record[2]), tuple(record[4]), (record[3], record[5], record[6]))

	                        	fout.write(str(record_mod))

                        		fout.write("\n")

                        	elif record[1] == "comm":

					record_mod = (record[1], record[2], tuple(record[3]), record[4])

	                        	fout.write(str(record_mod))

                        		fout.write("\n")

                            		try: record_n = ("call", record[2], "Send", (record[-1][0][1][0], traceDict[gid][pos+1][4][0]), record[-1][-2][-1])

                                        except: record_n = ("call", record[2], "Send", (record[-1][0][1][0], 0), record[-1][-2][-1]) # change! Its a bug. When there is no event after a blocking comm for a component, this will fail

                            		fout.write(str(record_n))

                            		fout.write("\n")

				pos = pos+1 

   
