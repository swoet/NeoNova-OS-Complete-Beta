# NeoNova OS Networking Stack

This directory contains the modular networking stack for NeoNova OS.

## Components
- **net_stack.[c/h]**: Main networking stack interface. Simulates packet flow.
- **net_if.[c/h]**: Network interface abstraction (init, send, recv).
- **net_proto.[c/h]**: Protocol stubs (Ethernet, ARP, IPv4, UDP, TCP). Modular registration.
- **net_sdn.[c/h]**: SDN controller stub.
- **net_vpn.[c/h]**: VPN module stub.
- **net_utils.[c/h]**: Utility/logging functions.

## Packet Structure
- `net_packet_t`: Generic structure for passing packets between layers.

## Simulated Flow
- On each tick, the stack creates a packet and passes it through the protocol layer, logging each step.

## Extending
- Implement real protocol logic and interface drivers as needed.
- Add more protocol layers or features (e.g., VLAN, container routing, etc.). 