import Sniper

def get_parser():
  import argparse
  parser = argparse.ArgumentParser(description='Run Example Algorithm.')
  parser.add_argument("--evtmax", type=int, default=10, help='events to be processed')
  parser.add_argument("--input", default="input.root", help='input')
  parser.add_argument("--output", default="output.root", help='output')
  return parser

if __name__ == "__main__":
  parser = get_parser()
  args = parser.parse_args()
  print args

  Sniper.loadDll("libSimEvent.so")
  Sniper.loadDll("libElecEvent.so")
  Sniper.loadDll("Linux-x86_64/libExampleAlg.so")

  task = Sniper.Task("task")
  #task.asTop()
  task.setEvtMax(args.evtmax)

  import BufferMemMgr
  bufMgr = task.createSvc("BufferMemMgr")

  import RootIOSvc
  ri = task.createSvc("RootInputSvc/InputSvc")
  ri.property("InputFile").set([args.input])

  task.createAlg("ExampleAlg")

  task.show()
  task.run()
