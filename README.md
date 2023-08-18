# draw-tree

A data structure that models randomly drawing from a bag of items.
You can assign weights to items to give them a higher or lower chance of being drawn.
You can also specify whether drawing an item prevents drawing that item or some other items in the future.

This was inspired by the problem of generating enchantments for a weapon in a videogame.
If there are a ton of possible enchantments relative to the number of enchantments a weapon would have,
it is not efficient to generate a permutation of all the possibilities, especially if we want to cancel items out based on arbitrary rules.

Given that the number of items (call this number $n$) is known at construction, we form a full binary tree deep enough
to have at least $n$ leaf nodes. This not only simplifies the code but also allows us to use a vector to represent the tree,
similar to a binary heap. We can use simple and fast arithmetic to calculate the index of a parent or child of a node in the vector.
The leaf nodes have the weight of the corresponding item. A non-leaf node
has weight equal to the sum of the weights of its children. Every time we pick an item, we roll a random integer in the range
[1, (weight of the root node)] and traverse down the tree. At each step we descend to the left child if our current roll is at most the weight
of the left child. Otherwise, we descend right and subtract the left child weight from our roll. When we hit a leaf node we deactivate all the items that the user wants to cancel out
and return the index we stopped at.
