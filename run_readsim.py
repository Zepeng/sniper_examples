#!/usr/bin/env python
# -*- coding:utf-8 -*-
# author: lintao

import Sniper

def get_parser():
	import argparse
	parser = argparse.ArgumentParser(description='Run nEXO Detector Simulation.')
	parser.add_argument("--evtmax", type=int, default=10, help='events to be processed')
	parser.add_argument("--input", default="sample-elecsim.root", help='input')
	parser.add_argument("--output", default="sample-tmvainput.root", help='output')


	return parser


if __name__ == "__main__":
	parser = get_parser()
	args = parser.parse_args()
	print args

	task = Sniper.Task("task")
	task.asTop()
	task.setEvtMax(args.evtmax)

	# = buffer =
	import BufferMemMgr
	bufMgr = task.createSvc("BufferMemMgr")
	bufMgr.property("TimeWindow").set([0, 0]);
	# = rootio =
	import RootIOSvc
	ri = task.createSvc("RootInputSvc/InputSvc")
	ri.property("InputFile").set([args.input])

	ros = task.createSvc("RootOutputSvc/OutputSvc")
	ros.property("OutputStreams").set({
		"/Event/Sim": args.output,
		})

	# = digitizer =
	Sniper.loadDll("Linux-x86_64/libtutorials.so")

	digitest = task.createAlg("ReadSimEvent")


	task.show()
	task.run()
