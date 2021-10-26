
import re

p = re.compile(r"""
([^a-zA-Z_])      #  ( ... ) --> \1  see groups in re
int
([^a-zA-Z0-9_])   #  ( ... ) --> \2
""",re.VERBOSE)

p_mc = re.compile(r"""
([^a-zA-Z_])      #  ( ... ) --> \1  see groups in re
DataArrayInt
([^a-zA-Z0-9_])   #  ( ... ) --> \2
""",re.VERBOSE)

p_vc = re.compile(r"""
([^a-zA-Z_])      #  ( ... ) --> \1  see groups in re
int_v
([^a-zA-Z0-9_])   #  ( ... ) --> \2
""",re.VERBOSE)

def replace(text):
    # print text
    # adding ' ' before and after if text begins or ends with 'int'
    text = " " + text + " "
    # \1 represents leading char and \2 trailing one
    # Loop avoids to call modifie twice. If no loop, this example fails to replace the int at the second line :
    # test='this is an int\nint is integer int'
    # print(p.sub(r'\1long\2',test))
    while True:
        newS = p.sub(r'\1long\2', text)
        if newS == text:
            break
        text = newS

    # replacing medcoupling arrays of int with arrays of long 
    while True:
        newS = p_mc.sub(r'\1DataArrayInt64\2', text)
        if newS == text:
            break
        text = newS

    # replacing vc vectors of int with vectors of long
    while True:
        newS = p_vc.sub(r'\1long_v\2', text)
        if newS == text:
            break
        text = newS

    # removing the added ' '
    new = newS[1:-1]
    print(new, end='')
    return

def main(filename):
    f = open(filename)
    content = f.read()
    f.close()
    replace(content)
    return

if __name__ == "__main__":
    from sys import argv
    main(argv[1])
    pass
