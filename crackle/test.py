import sys

for i, x in enumerate(sys.stdin):
    n = i + 1
    assert ("Crackle" in x) == (n % 3 == 0), f'{n}: {x}'
    assert ("Pop" in x) == (n % 5 == 0), f'{n}: {x}'
