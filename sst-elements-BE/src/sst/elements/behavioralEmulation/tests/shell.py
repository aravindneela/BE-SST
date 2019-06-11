"""
  A set of functions for text-based user interaction.

    Copyright (C) 2012-2015 Dylan Rudolph

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import math
import textwrap

class Shell(object):

    WARNING = "** ", " **"
    ERROR = "*** ", " ***"
    ALERT = "<<<<< ", " >>>>>"
    INSTRUCT = ">>> ", ""
    ONE_INDENT = "  "

    def __init__(self, line_length=79, verbose=False, suppress=False):
        """ """
        self.line_length = line_length
        self.suppressed = suppress
        self.verbose = verbose
        self.set_indent(0)

    def repeat(self, which, count):
        """Repeat a character ceil and floor 'count'/2 times, return both."""
        return which * int(math.ceil(count)), which * int(math.floor(count))

    # ----------------------- Indent-Related Functions ---------------------- #

    def set_indent(self, level):
        """Set the indentation level."""
        self.current_indent = max(level, 0)

    def increase_indent(self):
        """Increase the indentation level by 1."""
        self.set_indent(self.current_indent + 1)

    def decrease_indent(self):
        """Decrease the indentation level by 1."""
        self.set_indent(self.current_indent - 1)

    # -------------------------- Printing Functions ------------------------- #

    def say(self, text, indent=True, verbose=False, show=True):
        """Put some text in the prompt at the proper indent level."""
        indent_string = self.ONE_INDENT * self.current_indent
        if (not self.suppressed) and show:
            if (self.verbose and verbose) or verbose == False:
                print indent_string + text if indent else text

    def dashes(self, verbose=False):
        """Print dashes for a full line."""
        self.say("-" * self.line_length, indent=False, verbose=verbose)

    def newline(self, verbose=False):
        """Skip a line."""
        self.say("", indent=False, verbose=verbose)

    # ---------------------- Compound-Printing Functions -------------------- #

    def heading(self, text):
        """Print a heading-formatted string."""
        pre, suf = self.repeat("-", (self.line_length - 2 - len(text)) / 2.)
        self.newline()
        self.dashes()
        self.say(pre + " " + text + " " + suf, indent=False)
        self.dashes()

    def subheading(self, text):
        """Print a sub-heading-formatted string."""
        pre, suf = self.repeat("-", (self.line_length - 2 - len(text)) / 2.)
        self.newline()
        self.say(pre + " " + text + " " + suf, indent=False)
        self.newline()

    def alert(self, text):
        """Print an alert-formatted (centered) string."""
        pre, suf = self.repeat(" ", (self.line_length - 12 - len(text)) / 2.)
        self.say(pre+self.ALERT[0] + text + self.ALERT[1]+suf, indent=False)

    def center(self, text):
        """Print an centered string."""
        pre, suf = self.repeat(" ", (self.line_length - len(text)) / 2.)
        self.say(pre + text + suf, indent=False)

    def error(self, text, indent=False):
        """Print an error-formatted string."""
        self.say(self.ERROR[0] + text + self.ERROR[1], indent=indent)

    def warn(self, text, indent=False):
        """Print a warning-formatted string."""
        self.say(self.WARNING[0] + text + self.WARNING[1], indent=indent)

    def instruct(self, text, indent=False):
        """Print an instruction-formatted string."""
        self.say(self.INSTRUCT[0] + text + self.INSTRUCT[1], indent=indent)

    # ---------------------- User-Interaction Functions --------------------- #

    def pause(self, indent=False):
        """Request that the user press a key to continue."""
        indent_string = self.ONE_INDENT * self.current_indent
        text = "Press Enter To Continue...\n"
        raw_input(indent_string + text if indent else text)

    # ---------------------- Complex Printing Functions --------------------- #

    def description(self, heading, string, seperator=" :: ", stop=-1):
        """Print a description-formatted string."""
        indent_string = self.ONE_INDENT * self.current_indent
        heading = indent_string + heading + seperator
        wrapped = textwrap.wrap(string, self.line_length - len(heading))
        for index, line in enumerate(wrapped):
            _line = heading + line if index == 0 else " " * len(heading) + line
            print _line if index < (len(wrapped) - 1) else _line[:stop]

