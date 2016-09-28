#!/usr/bin/env python

# test class
class Person:
    def __init__(self, fname, lname):
        self._fname = fname
        self._lname = lname

    def __str__(self):
        return 'fname: ' + self._fname + ', lname: ' + self._lname

    def __repr__(self):
        return repr((self._fname, self._lname))
  
    def __del__(self):
        print('deleting Person: '+self.__repr__())


# main test program
def main():
  hans = Person('hans', 'ewetz')
  print(repr(hans))


if __name__ == '__main__':
    main()
