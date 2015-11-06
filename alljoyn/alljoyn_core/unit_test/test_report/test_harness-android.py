
# Copyright (c) 2012, AllSeen Alliance. All rights reserved.
#
#    Permission to use, copy, modify, and/or distribute this software for any
#    purpose with or without fee is hereby granted, provided that the above
#    copyright notice and this permission notice appear in all copies.
#
#    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

import getopt
import os
import re
import sys

def usage():
    print >> sys.stderr, """
Usage:
    python test_harness-script.py [ -c config_file ] [ -o scriptfile ] [ -t gtestfile ] [ -p path_to_gtestfile ]
where:
    config_file:    test_harness config file;       default: test_harness.conf
    scriptfile:     script file to write;           default: gtest.sh
    gtestfile:      name of gtest executable file;  default: ajtest
    path_to_gtestfile:   optional path to directory containing gtestfile (when scriptfile runs)
"""

def main(argv=None):

    # get commandline options

    conffile='test_harness.conf'
    scriptfile='gtest.sh'
    gtestfileopt=''
    testpath=''

    if argv is None:
        argv=[]
    if len(argv) > 0:
        try:
            opts, junk = getopt.getopt(argv, 'c:o:p:t:')
            if junk:
                print >> sys.stderr, 'error, unrecognized arguments ' + str(junk)
                usage()
                return 2
            for opt, val in opts:
                if opt == '-c':
                    conffile = val
                elif opt == '-o':
                    scriptfile = val
                elif opt == '-p':
                    testpath = val
                elif opt == '-t':
                    gtestfileopt = val
        except getopt.GetoptError, err:
            print >> sys.stderr, 'error, ' + str(err)
            usage()
            return 2

    # initialize

    dict = []
    filter = ''
    negfilter = ''
    gtestfile = ''
    part = ''
    text = ''

    re_comment = re.compile( r'\s*#.*$' )
    re_equals  = re.compile( r'\s*=\s*' )
    re_lastcolon   = re.compile( r':$' )
    re_TestCases   = re.compile( r'^\[\s*Test\s*Cases\s*\]', re.I )
    re_Environment = re.compile( r'^\[\s*Environment\s*\]', re.I )
    re_GTestFile   = re.compile( r'^\[\s*GTest\s*File\s*\]', re.I )

    # read config file one line at a time

    try:
        with open( conffile, 'r' ) as fileptr:

            for line in fileptr:
                # strip leading and trailing whitespace
                line = line.strip()
                line = line.strip('\n')
                # strip trailing comment (and preceding whitespace), if any
                line = re_comment.sub( '', line )

                # search line for part header
                if re_TestCases.search( line ):
                    # line ~= [ TestCases ]
                    part = 'TestCases'
                    continue

                elif re_Environment.search( line ):
                    # line ~= [ Environment ]
                    part = 'Environment'
                    print '[Environment]'
                    continue

                elif re_GTestFile.search( line ):
                    # line ~= [ GTestFile ]
                    part = 'GTestFile'
                    continue

                else:
                    # line is none of the above

                    # split line around equals sign (and surrounding whitespace), if any
                    dict = re_equals.split( line, 1 )

                    if (len(dict) > 1):
                        # line ~= something = something

                        if part == 'TestCases':
                            # Can select individual tests as well as groups.
                            # That is, TestCase selection can look like Foo.Bar=YES, not just Foo=YES.
                            # You can also used negative selection, like *=YES followed by Foo.Bar=NO.

                            d0 = dict[0].split('.',1)
                            if (dict[1].upper() == 'YES' or dict[1].upper() == 'Y'):
                                if (len(d0) > 1):
                                    filter = filter + dict[0] + ':'
                                else:
                                    filter = filter + dict[0] + '.*' + ':'
                            elif (dict[1].upper() == 'NO' or dict[1].upper() == 'N'):
                                if (len(d0) > 1):
                                    negfilter = negfilter + dict[0] + ':'
                                else:
                                    negfilter = negfilter + dict[0] + '.*' + ':'

                        elif part == 'Environment':
                            print '\t%s="%s"'             % ( dict[0], dict[1] )
                            text = text + '\n%s="%s"'     % ( dict[0], dict[1] )
                            text = text + '\nexport %s\n' % ( dict[0] )

                        elif part == 'GTestFile':
                            # the file name might contain = character
                            gtestfile = line

                    elif part == 'GTestFile' and line != '':
                        gtestfile = line

                    else:
                        # line is unusable
                        continue

    except IOError:
        print >> sys.stderr, 'error opening config file "%s"' % conffile
        return 2

    # assemble the path to gtestfile to execute

    command = gtestfile
    if gtestfileopt != '':
        command = gtestfileopt
    if command == '':
        command = 'ajtest'
    if testpath != '':
        text = text + '\ncd "%s" || exit 2\n' % testpath
        command = os.path.join( testpath, command )
    else:
        command = os.path.join( '.', command )

    print '[GTestFile]\n\t%s'             % command
    text = text + '\nls "%s" || exit 2\n' % command

    # assemble the gtest filter, if any

    if filter == '' and negfilter == '':
        pass
    elif filter != '' and negfilter == '':
        filter = re_lastcolon.sub( '', filter )
    elif filter == '' and negfilter != '':
        filter = '*' + '-' + re_lastcolon.sub( '', negfilter )
    else:
        filter = re_lastcolon.sub( '', filter ) + '-' + re_lastcolon.sub( '', negfilter)

    if filter != '':
        print '[TestCases]\n\t%s' % filter
        command = command + ' --gtest_filter="' + filter + '"'

    # script ends by executing the gtestfile with filter argument, if any

    text = text + '\n' + command + ' || exit 1'

    # write the scriptfile

    try:
        with open( scriptfile, 'w' ) as fileptr:

            print >> fileptr, text

    except IOError:
        print >> sys.stderr, 'error opening script file "%s"' % scriptfile
        return 2

    return 0

if __name__ == '__main__':
    if len(sys.argv) > 1:
        sys.exit(main(sys.argv[1:]))
    else:
        sys.exit(main())
