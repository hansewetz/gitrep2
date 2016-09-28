#!/usr/bin/env python
from collections import defaultdict

class KeyTracker(object):
    """ A value factory function to be used in 'defaultdict()' """

    def __init__(self):
        """ constructor """
        self._count = 0

    def __call__(self):
        """ callable function - returns an empty list """
        self._count += 1
        return []
    
    @property
    def count(self):
        """ returns #of times this function object has been called """
        return self._count

    @count.setter
    def count(self, val):
        """ setter property not supported - will throw exception """
        raise ValueError('cannot set \'count\' in class KeyTracker')


# main test program
def main():
    names = ['hans', 'claes']
    key_tracker = KeyTracker()
    dests = defaultdict(key_tracker);
    dests[names[0]].append('Stockholm');
    dests[names[0]].append('Luxembourg')
    for name in names :
        print name, "-->", dests[name]
    print '#newkeys: ', key_tracker.count


if __name__ == '__main__':
    main()
