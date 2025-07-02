#include "../widgets/orb.h"
#include "../widgets/infostream.h"
#include "../widgets/ai_orb.h"
#include "../widgets/dna_link.h"
#include "../widgets/quantum_timeline.h"
#include "../widgets/agent.h"
#include "../widgets/spatial.h"
#include "../widgets/swarm_collab.h"
#include "../widgets/self_heal.h"
#include "../widgets/dna_market.h"
#include <stdio.h>

void desktop_demo_orbs() {
    orb_widget_t* launcher_orb = orb_create(100, 600, 40, 0x00BFFF, "Launcher");
    orb_widget_t* notify_orb = orb_create(200, 600, 32, 0xFF8800, "Notify");
    orb_widget_t* ai_orb = orb_create(300, 600, 48, 0xAA00FF, "AI");
    orb_set_state(ai_orb, ORB_STATE_ACTIVE);
    orb_animate(launcher_orb, 0.3f);
    orb_animate(notify_orb, 0.7f);
    orb_render(launcher_orb);
    orb_render(notify_orb);
    orb_render(ai_orb);
    orb_set_state(notify_orb, ORB_STATE_NOTIFICATION);
    orb_render(notify_orb);
    orb_set_label(ai_orb, "Companion");
    orb_render(ai_orb);
    // Free orbs
    free(launcher_orb); free(notify_orb); free(ai_orb);
}

void desktop_demo_orbs_and_streams() {
    // Orbs
    orb_widget_t* launcher_orb = orb_create(100, 600, 40, 0x00BFFF, "Launcher");
    orb_widget_t* notify_orb = orb_create(200, 600, 32, 0xFF8800, "Notify");
    orb_widget_t* ai_orb = orb_create(300, 600, 48, 0xAA00FF, "AI");
    orb_set_state(ai_orb, ORB_STATE_ACTIVE);
    orb_animate(launcher_orb, 0.3f);
    orb_animate(notify_orb, 0.7f);
    orb_render(launcher_orb);
    orb_render(notify_orb);
    orb_render(ai_orb);
    orb_set_state(notify_orb, ORB_STATE_NOTIFICATION);
    orb_render(notify_orb);
    orb_set_label(ai_orb, "Companion");
    orb_render(ai_orb);
    // Info Streams
    infostream_widget_t* cpu_stream = infostream_create(0, 0, 400, 6, INFOSTREAM_ORIENT_TOP, 0x00FF00, "CPU");
    infostream_widget_t* net_stream = infostream_create(0, 794, 400, 6, INFOSTREAM_ORIENT_BOTTOM, 0x00BFFF, "NET");
    infostream_widget_t* notif_stream = infostream_create(1274, 0, 6, 800, INFOSTREAM_ORIENT_RIGHT, 0xFF8800, "NOTIF");
    infostream_set_data(cpu_stream, "CPU: 12% | Temp: 42C");
    infostream_set_data(net_stream, "NET: 1.2MB/s up | 3.4MB/s down");
    infostream_set_data(notif_stream, "3 new notifications");
    infostream_animate(cpu_stream, 0.5f);
    infostream_animate(net_stream, 0.2f);
    infostream_animate(notif_stream, 0.8f);
    infostream_render(cpu_stream);
    infostream_render(net_stream);
    infostream_render(notif_stream);
    // Free
    free(launcher_orb); free(notify_orb); free(ai_orb);
    free(cpu_stream); free(net_stream); free(notif_stream);
}

void desktop_demo_futuristic_suite() {
    // Orbs
    orb_widget_t* launcher_orb = orb_create(100, 600, 40, 0x00BFFF, "Launcher");
    orb_widget_t* notify_orb = orb_create(200, 600, 32, 0xFF8800, "Notify");
    orb_widget_t* ai_orb_legacy = orb_create(300, 600, 48, 0xAA00FF, "AI");
    orb_set_state(ai_orb_legacy, ORB_STATE_ACTIVE);
    orb_animate(launcher_orb, 0.3f);
    orb_animate(notify_orb, 0.7f);
    orb_render(launcher_orb);
    orb_render(notify_orb);
    orb_render(ai_orb_legacy);
    orb_set_state(notify_orb, ORB_STATE_NOTIFICATION);
    orb_render(notify_orb);
    orb_set_label(ai_orb_legacy, "Companion");
    orb_render(ai_orb_legacy);
    // Info Streams
    infostream_widget_t* cpu_stream = infostream_create(0, 0, 400, 6, INFOSTREAM_ORIENT_TOP, 0x00FF00, "CPU");
    infostream_widget_t* net_stream = infostream_create(0, 794, 400, 6, INFOSTREAM_ORIENT_BOTTOM, 0x00BFFF, "NET");
    infostream_widget_t* notif_stream = infostream_create(1274, 0, 6, 800, INFOSTREAM_ORIENT_RIGHT, 0xFF8800, "NOTIF");
    infostream_set_data(cpu_stream, "CPU: 12% | Temp: 42C");
    infostream_set_data(net_stream, "NET: 1.2MB/s up | 3.4MB/s down");
    infostream_set_data(notif_stream, "3 new notifications");
    infostream_animate(cpu_stream, 0.5f);
    infostream_animate(net_stream, 0.2f);
    infostream_animate(notif_stream, 0.8f);
    infostream_render(cpu_stream);
    infostream_render(net_stream);
    infostream_render(notif_stream);
    // AI Companion Orb
    ai_orb_widget_t* ai_orb = ai_orb_create(1200, 700, 56, 0xAA00FF, "AI Companion");
    ai_orb_render(ai_orb);
    ai_orb_listen(ai_orb, true);
    ai_orb_render(ai_orb);
    ai_orb_set_state(ai_orb, AI_ORB_STATE_THINKING);
    ai_orb_animate(ai_orb, 0.6f);
    ai_orb_render(ai_orb);
    ai_orb_suggest(ai_orb, "Would you like to optimize your workflow?");
    ai_orb_render(ai_orb);
    ai_orb_listen(ai_orb, false);
    ai_orb_set_state(ai_orb, AI_ORB_STATE_IDLE);
    ai_orb_render(ai_orb);
    // DNA Link Demo
    printf("\n--- DNA Link Demo ---\n");
    int winA = 1, winB = 2;
    dna_link_t* link = dna_link_create(winA, winB, DNA_LINK_TYPE_HYBRID, "Sync+Action Link");
    dna_link_render(link);
    dna_link_attach(link);
    dna_link_render(link);
    dna_link_perform_action(link);
    dna_link_render(link);
    dna_link_detach(link);
    dna_link_render(link);
    // Collaboration Bubble Demo
    printf("\n--- Collaboration Bubble Demo ---\n");
    collab_bubble_t* bubble = collab_bubble_create(winA);
    collab_bubble_add_user(bubble, 101, "Alice");
    collab_bubble_add_user(bubble, 102, "Bob");
    collab_bubble_set_pointer(bubble, 101, 120, 200, false);
    collab_bubble_set_pointer(bubble, 102, 300, 400, true);
    collab_bubble_annotate(bubble, "Let's review this section together.");
    collab_bubble_render(bubble);
    collab_bubble_remove_user(bubble, 101);
    collab_bubble_render(bubble);
    // Quantum Timeline Demo
    printf("\n--- Quantum Timeline Demo ---\n");
    quantum_timeline_t* timeline = quantum_timeline_create(winA);
    quantum_timeline_save(timeline, "Initial", "State0");
    quantum_timeline_save(timeline, "Edited", "State1");
    quantum_timeline_save(timeline, "Final", "State2");
    quantum_timeline_render(timeline);
    quantum_timeline_rewind(timeline, 1);
    quantum_timeline_render(timeline);
    // Self-Healing Timeline Demo
    printf("\n--- Self-Healing Timeline Demo ---\n");
    self_heal_monitor_t heal;
    self_heal_monitor_init(&heal, winA, timeline);
    for (int i = 0; i < 5; ++i) {
        bool unstable = (i == 1 || i == 2 || i == 3); // Simulate instability
        self_heal_monitor_tick(&heal, unstable);
        quantum_timeline_render(timeline);
    }
    self_heal_trigger_rewind(&heal, 0);
    quantum_timeline_render(timeline);
    // Living Desktop AI Micro-Agents
    printf("\n--- Living Desktop AI Micro-Agents Demo ---\n");
    agent_manager_t agent_mgr;
    agent_manager_init(&agent_mgr);
    agent_create(&agent_mgr, AGENT_TYPE_OPTIMIZER, "Optimize CPU usage", 50, 100, 0x00FF00);
    agent_create(&agent_mgr, AGENT_TYPE_SUGGESTER, "Suggest break", 120, 180, 0xFF00FF);
    agent_create(&agent_mgr, AGENT_TYPE_MONITOR, "Monitor network", 200, 220, 0x00BFFF);
    agent_create(&agent_mgr, AGENT_TYPE_HEALTH, "Watch for instability", 300, 150, 0xFF8800);
    agent_create(&agent_mgr, AGENT_TYPE_SOCIAL, "Connect with peers", 400, 250, 0xFFD700);
    for (int t = 0; t < 3; ++t) {
        agent_manager_tick(&agent_mgr);
        for (int i = 0; i < agent_mgr.agent_count; ++i) agent_render(&agent_mgr.agents[i]);
    }
    // AR/VR/Spatial API Demo
    printf("\n--- AR/VR/Spatial API Demo ---\n");
    spatial_manager_t spatial_mgr;
    spatial_manager_init(&spatial_mgr);
    spatial_object_t* win_obj = spatial_object_create(&spatial_mgr, SPATIAL_TYPE_WINDOW, "Main Window", 0.0f, 0.0f, 0.0f);
    spatial_object_t* orb_obj = spatial_object_create(&spatial_mgr, SPATIAL_TYPE_ORB, "Launcher Orb", 1.0f, 0.5f, 0.2f);
    spatial_object_t* agent_obj = spatial_object_create(&spatial_mgr, SPATIAL_TYPE_AGENT, "Optimizer Agent", -0.5f, 0.8f, 0.3f);
    spatial_object_t* stream_obj = spatial_object_create(&spatial_mgr, SPATIAL_TYPE_STREAM, "CPU Stream", 0.2f, -0.3f, 0.1f);
    spatial_object_render(win_obj);
    spatial_object_render(orb_obj);
    spatial_object_render(agent_obj);
    spatial_object_render(stream_obj);
    spatial_object_move(win_obj, 2.0f, 1.0f, 0.5f);
    spatial_object_move(orb_obj, 1.5f, 0.7f, 0.4f);
    spatial_object_render(win_obj);
    spatial_object_render(orb_obj);
    spatial_open_portal(&spatial_mgr, win_obj->id, orb_obj->id);
    spatial_object_render(win_obj);
    // Swarm Collaboration Demo
    printf("\n--- Swarm Collaboration Demo ---\n");
    swarm_session_t swarm;
    swarm_collab_init(&swarm, 42);
    swarm_collab_add_user(&swarm, 101, "Alice", true);
    swarm_collab_add_user(&swarm, 102, "Bob", false);
    swarm_collab_merge_desktops(&swarm, 1);
    swarm_collab_merge_desktops(&swarm, 2);
    swarm_collab_federate_ai(&swarm);
    swarm_collab_render(&swarm);
    // App DNA Marketplace Demo
    printf("\n--- App DNA Marketplace Demo ---\n");
    dna_market_t market;
    dna_market_init(&market);
    int id1 = dna_market_add(&market, "Text Editor DNA", DNA_LINK_TYPE_DATA, "edit,save,undo");
    int id2 = dna_market_add(&market, "Image Viewer DNA", DNA_LINK_TYPE_DATA, "view,zoom,rotate");
    int id3 = dna_market_add(&market, "Cloud Sync DNA", DNA_LINK_TYPE_ACTION, "sync,share,backup");
    dna_market_list(&market);
    int id4 = dna_market_merge(&market, id1, id3, "Editor+Cloud Hybrid");
    dna_market_render(&market);
    // Free
    free(launcher_orb); free(notify_orb); free(ai_orb_legacy);
    free(cpu_stream); free(net_stream); free(notif_stream);
    free((void*)ai_orb->label); free((void*)ai_orb->suggestion); free(ai_orb);
    free((void*)link->label); free(link);
    free(bubble);
    free(timeline);
} 