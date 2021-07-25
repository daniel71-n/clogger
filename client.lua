#!/usr/bin/lua

-- unix socket functionality is in the .unix submodule
-- #s = require("socket.unix")
-- create socket
-- send stdin through unix socket

s = require"socket.unix"
c = assert(s.dgram())
--c = assert(s.stream())
-- or, s.stream() to get a stream socket, which is connection-oriented, : s.stream()

-- print functions in the socket.unix package
--for k,v in pairs(socket.unix) do
--    print(tostring(k),tostring(v))
--end

-- both stream and dgram sockets need the connect step
assert(c:connect("/tmp/dnsock"))

-- the program takes a single argument: a string to send through the unix domain socket
c:send(arg[1])

--[[ 
-- * * * * * * * SOME NOTES * * * * * * * 
-- While it would be easy to find documentation for TCP/UDP sockets, the documentation for unix domain sockets
-- if scarce (nonexistent, really).
-- However, the above works. 

-- The server I'm trying to connect to is of type DGRAM, while here I originally created a stream
-- socket object. WHen tryin to connect, I was getting the following error:
-- 'Protocol wrong type for socket'
-- The solution was to crete a DGRAM socket object instead, which worked.


-- here's an example taken from online:

--  -- send stdin through unix socket
    socket = require"socket"
    socket.unix = require"socket.unix"
    c = assert(socket.unix())
    assert(c:connect("/tmp/foo"))
    while 1 do
        local l = io.read()
        assert(c:send(l .. "\n"))
    end'

--]]
