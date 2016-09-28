#!/usr/bin/env python

import collections

# create a class holding Person
def create_person_class():
    """ create a Person class """

    # create a new class
    ret = collections.namedtuple('Person',('fname', 'lname', 'ssn'))

    # define a dump method
    def dump(self):
        print('name: %s %s, ssn: %s' % (self.fname, self.lname, self.ssn))

    # add dump method to class
    ret.dump = dump
    return ret


# main test program
def main():
    Person = create_person_class();
    hans = Person('Hans', 'Ewetz', '591049472')
    hans.dump()

if __name__ == '__main__':
    main()
