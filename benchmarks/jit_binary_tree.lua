local luamir = require 'luamir'
local JitBottomUpTree
local function BottomUpTree(item, depth)
    if depth > 0 then
      local i = item + item
      depth = depth - 1
      local left, right = JitBottomUpTree(i-1, depth), JitBottomUpTree(i, depth)
      return { item, left, right }
    else
      return { item }
    end
  end

  JitBottomUpTree = luamir.ljit(BottomUpTree)
  BottomUpTree = JitBottomUpTree

  local JitItemCheck
  local function ItemCheck(tree)
    if tree[2] then
      return tree[1] + JitItemCheck(tree[2]) - JitItemCheck(tree[3])
    else
      return tree[1]
    end
  end
  JitItemCheck = luamir.ljit(ItemCheck)
  ItemCheck = JitItemCheck
  
  local t1 = os.clock()

  local N = tonumber(arg and arg[1]) or 0
  local mindepth = 4
  local maxdepth = mindepth + 2
  if maxdepth < N then maxdepth = N end
  
  do
    local stretchdepth = maxdepth + 1
    local stretchtree = BottomUpTree(0, stretchdepth)
    io.write(string.format("stretch tree of depth %d\t check: %d\n",
      stretchdepth, ItemCheck(stretchtree)))
  end
  
  local longlivedtree = BottomUpTree(0, maxdepth)
  
  for depth=mindepth,maxdepth,2 do
    local iterations = 2 ^ (maxdepth - depth + mindepth)
    local check = 0
    for i=1,iterations do
      check = check + ItemCheck(BottomUpTree(1, depth)) +
              ItemCheck(BottomUpTree(-1, depth))
    end
    io.write(string.format("%d\t trees of depth %d\t check: %d\n",
      iterations*2, depth, check))
  end
  
  io.write(string.format("long lived tree of depth %d\t check: %d\n",
    maxdepth, ItemCheck(longlivedtree)))

    local t2 = os.clock()
    print("time: ", t2 - t1)