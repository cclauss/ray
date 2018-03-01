#ifndef LS_POLICY_CC
#define LS_POLICY_CC
#include <unordered_map>

#include "scheduling_policy.h"
#include "scheduling_resources.h"

using namespace std;
namespace ray{

/// Given a set of cluster resources, produce a placement decision on all work
/// in the queue.
std::unordered_map<TaskID, ClientID, UniqueIDHasher> SchedulingPolicy::Schedule(
    const std::unordered_map<ClientID, SchedulingResources, UniqueIDHasher> &cluster_resources) {
  // return data structure
  std::unordered_map<TaskID, ClientID, UniqueIDHasher> decision;

  // Iterate over running tasks, get their resource demand and try to schedule.
  TaskID last_task_id = TaskID::nil();
  const auto ready_tasks = sched_queue_.ready_tasks();
  for (const auto &t : ready_tasks) {
    // Get task's resource demand
    const auto &resource_demand_map = t.GetTaskSpecification().GetRequiredResources();
    const auto &resource_demand = ResourceSet(resource_demand_map);
    // TODO(atumanov): iterate over cluster_resources and locate available capacity.
    SchedulingResources resource_supply = cluster_resources.at(ClientID::nil());
    ResourceSet resource_supply_set = resource_supply.GetAvailableResources();
    bool task_feasible = resource_demand.isSubset(resource_supply_set);
    if (task_feasible) {
      last_task_id = t.GetTaskSpecification().TaskId();
      decision[last_task_id] = ClientID::nil();
    }
  }
  return decision;
}

} // end namespace ray
#endif