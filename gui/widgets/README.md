# NeoNova OS Widget Library

## Orb Widget (Futuristic Contextual Orb)

- Animated, morphing, interactive orb for HoloFlow UI
- Supports states: idle, active, morphing, notification, dragging
- Can be focused, animated, and morph between shapes
- Used for app launchers, quick actions, notifications, and AI companion

### API
- `orb_widget_t* orb_create(int x, int y, int radius, unsigned int color, const char* label);`
- `void orb_render(const orb_widget_t* orb);`
- `void orb_set_state(orb_widget_t* orb, orb_state_t state);`
- `void orb_set_action(orb_widget_t* orb, void (*on_action)(orb_widget_t*, void*), void* user_data);`
- `void orb_animate(orb_widget_t* orb, float target_morph);`
- `void orb_morph(orb_widget_t* orb, float morph);`
- `void orb_set_label(orb_widget_t* orb, const char* label);`

### Usage
- Place orbs on the desktop, in window chrome, or as persistent AI companions
- Orbs can morph, animate, and respond to user actions (click, drag, voice, gesture)

## InfoStream Widget (Ambient Info Stream)

- Animated, live info stream for window edges (top, bottom, left, right)
- Shows system, app, network, or resource info in real time
- Can be animated, colored, and labeled

### API
- `infostream_widget_t* infostream_create(int x, int y, int length, int thickness, infostream_orientation_t orientation, unsigned int color, const char* label);`
- `void infostream_render(const infostream_widget_t* stream);`
- `void infostream_set_data(infostream_widget_t* stream, const char* data);`
- `void infostream_animate(infostream_widget_t* stream, float phase);`

### Usage
- Attach to window edges or desktop for live, ambient feedback
- Animate to show activity, alerts, or transitions

## AI Companion Orb Widget

- Persistent, context-aware, animated AI orb for help, suggestions, and automation
- Supports states: idle, listening, thinking, suggesting, acting
- Can listen, animate, and provide suggestions or actions

### API
- `ai_orb_widget_t* ai_orb_create(int x, int y, int radius, unsigned int color, const char* label);`
- `void ai_orb_render(const ai_orb_widget_t* orb);`
- `void ai_orb_set_state(ai_orb_widget_t* orb, ai_orb_state_t state);`
- `void ai_orb_listen(ai_orb_widget_t* orb, bool enable);`
- `void ai_orb_suggest(ai_orb_widget_t* orb, const char* suggestion);`
- `void ai_orb_animate(ai_orb_widget_t* orb, float phase);`

### Usage
- Place on desktop or in window chrome as a persistent AI companion
- Use to provide help, suggestions, and automate workflows

## Window DNA Link System

- Link windows for data, actions, or hybrid automation
- Drag-and-drop or programmatically create links between windows
- Supports attach, detach, and perform action

### API
- `dna_link_t* dna_link_create(int source_id, int target_id, dna_link_type_t type, const char* label);`
- `void dna_link_render(const dna_link_t* link);`
- `void dna_link_attach(dna_link_t* link);`
- `void dna_link_detach(dna_link_t* link);`
- `void dna_link_perform_action(dna_link_t* link);`

### Usage
- Drag one window's DNA onto another to link data, automate actions, or create hybrid apps
- Use for workflow automation, data sync, or app fusion

## Collaboration Bubble System

- Share windows in real-time with other users
- Live pointer, voice, and annotation overlays
- Add/remove users, annotate, and set remote pointers

### API
- `collab_bubble_t* collab_bubble_create(int window_id);`
- `void collab_bubble_render(const collab_bubble_t* bubble);`
- `void collab_bubble_add_user(collab_bubble_t* bubble, int user_id, const char* name);`
- `void collab_bubble_remove_user(collab_bubble_t* bubble, int user_id);`
- `void collab_bubble_annotate(collab_bubble_t* bubble, const char* annotation);`
- `void collab_bubble_set_pointer(collab_bubble_t* bubble, int user_id, int x, int y, bool voice);`

### Usage
- Bubble a window to share it live, collaborate with remote users, and annotate in real time

## Quantum Timeline System

- Rewind, branch, or parallelize window/app states
- Save, rewind, and branch timelines for any window
- Run multiple timelines in parallel

### API
- `quantum_timeline_t* quantum_timeline_create(int window_id);`
- `void quantum_timeline_save(quantum_timeline_t* timeline, const char* label, const char* snapshot);`
- `void quantum_timeline_rewind(quantum_timeline_t* timeline, int state_id);`
- `quantum_timeline_t* quantum_timeline_branch(quantum_timeline_t* timeline, const char* label);`
- `void quantum_timeline_render(const quantum_timeline_t* timeline);`

### Usage
- Instantly rewind or branch any app/window to a previous state
- Run multiple timelines in parallel for experimentation or recovery

## Living Desktop AI Micro-Agents

- Adaptive, visual, interactive agents for optimization, suggestion, monitoring, health, and social features
- Agents are animated, have goals, and interact with the user and each other

### API
- `void agent_manager_init(agent_manager_t* mgr);`
- `agent_t* agent_create(agent_manager_t* mgr, agent_type_t type, const char* goal, int x, int y, unsigned int color);`
- `void agent_manager_tick(agent_manager_t* mgr);`
- `void agent_render(const agent_t* agent);`

### Usage
- Place agents on the desktop as motes, swirls, or digital flora
- Agents learn, adapt, and visually respond to user/system state

## AR/VR/Spatial API

- 3D abstraction for windows, orbs, agents, streams, and portals
- Move, pin, and open HoloPortals between objects in AR/VR space
- Supports spatial state, orientation, and portal linking

### API
- `void spatial_manager_init(spatial_manager_t* mgr);`
- `spatial_object_t* spatial_object_create(spatial_manager_t* mgr, spatial_type_t type, const char* label, float x, float y, float z);`
- `void spatial_object_move(spatial_object_t* obj, float x, float y, float z);`
- `void spatial_object_render(const spatial_object_t* obj);`
- `void spatial_open_portal(spatial_manager_t* mgr, int from_id, int to_id);`

### Usage
- Move windows/orbs/agents in 3D, pin to locations, open portals for AR/VR or remote presence

## Swarm Collaboration

- Multi-user, multi-desktop merging for collaborative sessions
- Federated AI: pool knowledge and suggestions across users
- Add/remove users, merge desktops, federate AI, and render session state

### API
- `void swarm_collab_init(swarm_session_t* session, int session_id);`
- `void swarm_collab_add_user(swarm_session_t* session, int user_id, const char* name, bool ai_enabled);`
- `void swarm_collab_merge_desktops(swarm_session_t* session, int desktop_id);`
- `void swarm_collab_federate_ai(swarm_session_t* session);`
- `void swarm_collab_render(const swarm_session_t* session);`

### Usage
- Merge desktops for group work, enable federated AI for collective intelligence

## Self-Healing Timeline

- Auto-rewind windows/apps to healthy states on instability
- Health monitor agent tracks instability and triggers timeline actions
- Manual rewind supported

### API
- `void self_heal_monitor_init(self_heal_monitor_t* mon, int window_id, quantum_timeline_t* timeline);`
- `void self_heal_monitor_tick(self_heal_monitor_t* mon, bool is_unstable);`
- `void self_heal_trigger_rewind(self_heal_monitor_t* mon, int state_id);`

### Usage
- Attach to any window/app with a Quantum Timeline
- Monitor for instability and auto-rewind as needed

## App DNA Marketplace

- Export/import, share, and merge DNA links for apps and workflows
- Create new hybrid apps by fusing DNA from multiple sources
- List, merge, and render marketplace entries

### API
- `void dna_market_init(dna_market_t* market);`
- `int dna_market_add(dna_market_t* market, const char* label, dna_link_type_t type, const char* data);`
- `void dna_market_list(const dna_market_t* market);`
- `int dna_market_merge(dna_market_t* market, int id1, int id2, const char* new_label);`
- `void dna_market_render(const dna_market_t* market);`

### Usage
- Export/import DNA links, browse marketplace, merge DNA, and create new hybrid apps
