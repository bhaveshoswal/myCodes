import os
'''
The function accepts any path and finds the files which 
having 0kb size and deletes it if permision is there
'''


def clean(path, filename=False):
    l = os.listdir(path)
    print("Path : ", path)
    print("Total files are : ", len(l))
    empty = [x for x in l if os.path.getsize(x) == 0]
    print("Total files with 0 size : ", len(empty))
    input = raw_input("Do you want delete empty files (y/n) : ")
    if(input == 'y'):
        map(os.remove, empty)
    if(filename):
        return empty
    pass
