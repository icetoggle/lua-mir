local function Ack(M, N)
    if (M == 0) then
        return N + 1
    end
    if (N == 0) then
        return Ack(M - 1, 1)
    end
    return Ack(M - 1, Ack(M, (N - 1)))
end
local t1 = os.clock()
N = tonumber((arg and arg[1])) or 3
M = tonumber((arg and arg[2])) or 4
print(string.format("Ack(%d, %d) = %d\n", N, M, Ack(N,M)))
local t2 = os.clock()
print("time: ", t2 - t1)