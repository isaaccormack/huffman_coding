import sys

curr_ascii = 48

with open(sys.argv[1], 'r') as fileobj:
    f = open(sys.argv[1] + "_clean.txt", "w")
    for line in fileobj:  
       for ch in line:
            if ord(ch) < 0 or ord(ch) > 127:
                # write a standard distribution of ascii chars
                f.write(chr(curr_ascii))
                print("replaced: " + ch + " with: " + chr(curr_ascii))
                curr_ascii = curr_ascii + 1
                if curr_ascii > 122:
                    curr_ascii = 48
            else:
                f.write(ch)