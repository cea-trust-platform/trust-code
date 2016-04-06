
import re

p = re.compile(r"""
([^a-zA-Z_])      #  ( ... ) --> \1  see groups in re
int
([^a-zA-Z0-9_])   #  ( ... ) --> \2
""",re.VERBOSE)

def replace(text):
    # print text
    # adding ' ' before and after if text begins or ends with 'int'
    text = " " + text + " "
    # \1 represents leading char and \2 trailing one
    new = p.sub(r'\1long\2', text)
    # removing the added ' '
    new = new[1:-1]
    print new,
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
