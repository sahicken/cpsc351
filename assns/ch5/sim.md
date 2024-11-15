# ch5-hw-cpu-api (SIMULATION + CODE) (26 pts / 2 pts each)

## Homework (Simulation)

This simulation homework focuses on fork.py, a simple process creation simulator that shows how processes are related in a single “familial” tree.  Read the relevant README for details about how to run the simulator.

`cpu_homework/cpu-api % ./fork.py -s 10 -c`

1. Run ./fork.py -s 10 and see which actions are taken. Can you predict what the process tree looks like at each step? Use the -c flag to check your answers. Try some different random seeds (-s) or add more actions (-a) to get the hang of it.

YES, this is the first answer. FORK creates a single process off of the parent process (despite its name of ‘forking’.)

ARG seed 10
ARG fork_percentage 0.7
ARG actions 5
ARG action_list
ARG show_tree False
ARG just_final False
ARG leaf_only False
ARG local_reparent False
ARG print_style fancy
ARG solve True
Process Tree:
a
Action: a forks b
a
└── b
Action: a forks c
a
├── b
└── c
Action: c EXITS
a
└── b
Action: a forks d
a
├── b
└── d
Action: a forks e
a
├── b
├── d
└── e

2. One control the simulator gives you is the forkpercentage, controlled by the -f flag. The higher it is, the more likely the next action is a fork; the lower it is, the more likely the action is an exit. Run the simulator with a large number of actions (e.g., -a 100) and vary the forkpercentage from 0.1 to 0.9. What do you think the resulting final process trees will look like as the percentage changes? Check your answer with -c.

Higher percentages create deeper trees, while lower percentages create shallower trees. A tree shows the lineage of the "family", so more "grand, grand, ..., -parents" are present with the higher percentage of forking.

3. Now, switch the output by using the -t flag (e.g., run ./fork.py -t). Given a set of process trees, can you tell which actions were taken?

EXITs remove a node and relink children with the eldest process and forks add a child node to a (new or now) parent node (adding depth).

4. One interesting thing to note is what happens when a child exits;what happens to its children in the process tree? To study this, let’s create a specific example: ./fork.py -A a+b,b+c,c+d,c+e,c-. This example has process ’a’ create ’b’, which in turn creates ’c’, which then creates ’d’ and ’e’. However, then, ’c’ exits. What do you think the process tree should like after the exit? What if you use the -R flag? Learn more about what happens to orphaned processes on your own to add more context.

Without the `-R` flag, all child processes (recursively and all the way down), and withe the `-R` flag children processes unit with their grandparents.

5. One last flag to explore is the -F flag, which skips intermediate steps and only asks to fill in the final process tree. Run ./fork.py -F and see if you can write down the final tree by looking at the series of actions generated. Use different random seeds to try this a few times.

DONE

6. Finally, use both -t and -F together. This shows the final process tree, but then asks you to fill in the actions that took place. By looking at the tree, can you determine the exact actions that took place? In which cases can you tell? In which can’t you tell? Try some different random seeds to delve into this question.

You can roughly determine which actions take place, but not which order, particularly lateral traversal (same level of depth), can vary in any order. You also can't tell which "groups" of depths happen when, or the exact nature of any EXITs (maybe just if they happened or not).
