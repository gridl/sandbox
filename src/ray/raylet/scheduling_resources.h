#ifndef SCHEDULING_RESOURCES_H
#define SCHEDULING_RESOURCES_H

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "worker_pool.h"

namespace ray {

/// Resource availability status reports whether the resource requirement is
/// (1) infeasible, (2) feasible but currently unavailable, or (3) available.
typedef enum {
  kInfeasible,          ///< Cannot ever satisfy resource requirements.
  kResourcesUnavailable,///< Feasible, but not currently available.
  kFeasible             ///< Feasible and currently available.
} ResourceAvailabilityStatus;

/// \class ResourceSet
/// \brief Encapsulates and operates on a set of resources, including CPUs,
/// GPUs, and custom labels.
class ResourceSet {
public:

  /// \brief empty ResourceSet constructor.
  ResourceSet();

  /// \brief Constructs ResourceSet from the specified resource map.
  ResourceSet(const std::unordered_map<std::string, double> &resource_map);

  /// \brief Empty ResourceSet destructor.
  ~ResourceSet();

  /// \brief Test equality with the other specified ResourceSet object.
  /// \param rhs: Right-hand side object for equality comparison.
  /// \return True if objects are equal, False otherwise.
  bool operator==(const ResourceSet &rhs) const;

  /// \brief Test equality with the other specified ResourceSet object.
  /// \param other: Right-hand side object for equality comparison.
  /// \return True if objects are equal, False otherwise.
  bool IsEqual(const ResourceSet &other) const;

  /// \brief Test whether this ResourceSet is a subset of the other ResourceSet.
  /// \param other: The resource set we check being a subset of.
  /// \return True if the current resource set is the subset of other. False
  ///          otherwise.
  bool isSubset(const ResourceSet &other) const;

  /// \brief Test if this ResourceSet is a superset of the other ResourceSet.
  /// \param other: The resource set we check being a superset of.
  /// \return True if the current resource set is the superset of other.
  ///         False otherwise.
  bool isSuperset(const ResourceSet &other) const;


  /// \brief Add a new resource to the resource set.
  /// \param resource_name: name/label of the resource to add.
  /// \param capacity: numeric capacity value for the resource to add.
  /// \return True, if the resource was successfully added. False otherwise.
  bool AddResource(const std::string &resource_name, double capacity);

  /// \brief Remove the specified resource from the resource set.
  /// \param resource_name: name/label of the resource to remove.
  /// \return True, if the resource was successfully removed. False otherwise.
  bool RemoveResource(const std::string &resource_name);

  /// \brief Add a set of resources to the current set of resources.
  /// \param other: The other resource set to add.
  /// \return True if the resource set was added successfully. False otherwise.
  bool AddResources(const ResourceSet &other);

  /// \brief Subtract a set of resources from the current set of resources.
  /// \param other: The resource set to subtract from the current resource set.
  /// \return True if the resource set was subtracted successfully.
  ///         False otherwise.
  bool SubtractResources(const ResourceSet &other);

  /// Return the capacity value associated with the specified resource.
  /// \param[in] resource_name: Resource name for which capacity is requested.
  /// \param[out] value: Resource capacity value.
  /// \return True if the resource capacity value was successfully retrieved.
  ///         False otherwise.
  bool GetResource(const std::string &resource_name, double *value) const;

private:
  /// Resource capacity map.
  std::unordered_map<std::string, double> resource_capacity_;
};


/// SchedulingResources class encapsulates state of all local resources and manages
/// accounting of those resources. Resources include configured resource
/// bundle capacity, and GPU allocation map.
class SchedulingResources {
 public:
  // Raylet resource object constructors: set the total configured resource
  // capacity
  SchedulingResources() : resources_total_(ResourceSet()), resources_available_(ResourceSet()) {}

  SchedulingResources(const ResourceSet& total):
    resources_total_(total), resources_available_(total) {}

  SchedulingResources(const ResourceSet &total, const WorkerPool &pool):
    resources_total_(total), resources_available_(total) {}

  ResourceAvailabilityStatus CheckResourcesSatisfied(ResourceSet &resources) const;

  const ResourceSet &GetAvailableResources() const;

  /// Methods that mutate state.
  bool Release(const ResourceSet &resources);
  bool Acquire(const ResourceSet &resources);
 private:
   // static resource configuration (e.g., static_resources)
  ResourceSet resources_total_;
   // dynamic resource capacity (e.g., dynamic_resources)
  ResourceSet resources_available_;
   // gpu_map - replace with ResourceMap (for generality)
};

} // namespace ray

#endif // SCHEDULING_RESOURCES_H