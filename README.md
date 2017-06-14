# SO
University project for Operating Systems

## What am I looking at?
This project is inspired by a Stream Processing network like [Apache Storm][1]. It simulates a network of components to filter, modify and process several events. It is meant for Linux systems, and it introduces concepts such as forks, pipes and signals. It is able to run several [commands](https://github.com/p3rsephone/SO-Project/wiki/Commands). You can thank [p3rsephone](https://github.com/p3rsephone) for this beauty of a readme on the commands.

## How do I make it work? Do I say "hi siri!"?
Not yet, but we will take note of that. Use `make build` and then `./controller`.

## Do I need to do something for it to work?
Yes, it's not automated yet (and probably never will). You can type all of your [commands](https://github.com/p3rsephone/SO-Project/wiki/Commands) in the controller.

## Do I need to clean up my mess after I run it?
No you do not. With every `make build` you clean it all up before recompiling. If you just feel like deleting temp files then use `make clean`, on the house.

## Stuff that is probably (not) going to change
- [ ] Make inject keep writing to a node while it has something to write
- [ ] Stop the end node from dying when disconnecting 2 nodes
- [ ] Make it brew fresh coffee
- [ ] Make it respond to "hi siri!"

## Who made this? I need to give each of them a handshake
Well, thank these guys. They are awesome.

[Hugo Brandão (Me!)](https://github.com/jhugobb)

[Sérgio Alves](https://github.com/a-sac)

[Tiago Alves](https://github.com/tdaa)


[1]: https://storm.apache.org/
