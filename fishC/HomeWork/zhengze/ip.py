#!/usr/bin/python3.4

import re





if __name__ == '__main__':
	s1 = re.search(r'(([01]{0,1}\d{0,1}\d|2[0-4]\d|25[0-5])\.){3}([01]{0,1}\d{0,1}\d|2[0-4]\d|25[0-5])','191168.0.35')

	print(s1)
