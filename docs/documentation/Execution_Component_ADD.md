# Application Execution Component Architecture Design Document

| Field | Value |
|---|---|
| Document ID | ADD-EXEC-001 |
| Component | Application Execution (`Exec`) |
| Status | Draft |
| Version | 0.1 |
| Date | 2026-08-28 |
| Target | STM32N6, FreeRTOS |

## 1. Purpose

This Architecture Design Document (ADD) defines the intended architecture of
the application Execution component (`Exec`). The component provides the
system-level execution integration between statically configured software
component runnables and FreeRTOS tasks, queues, notifications, and mutual
exclusion primitives.

The design separates:

- component algorithms and runnable semantics;
- application-specific execution configuration;
- generated task bodies and typed communication ports;
- FreeRTOS scheduling and synchronization mechanisms.

The design is intended for a hard real-time, safety-oriented embedded system.
It prioritizes static configuration, bounded behavior, analyzability, explicit
failure handling, and generation from machine-readable sources of truth.

## 2. Scope

This revision covers:

- periodic run-to-completion runnables;
- event-driven runnables activated by queues or notifications;
- statically configured work-server tasks;
- statically allocated FreeRTOS tasks and queues;
- generated typed producer ports;
- serialized ownership of mutable component state;
- explicit use of FreeRTOS mutexes where serialization cannot remove shared
  mutable access;
- configuration validation and generation requirements;
- startup and runtime supervision responsibilities.

This revision assumes direct FreeRTOS use inside `Exec`, generated execution
integration code, and `cfg_exec`. An operating-system abstraction layer is not
part of this revision.

## 3. Non-goals

The following are deliberately outside the initial scope:

- portability to another RTOS;
- dynamic creation or deletion of application tasks at runtime;
- runtime registration of arbitrary runnables or work-item callbacks;
- dynamic memory allocation by `Exec` after startup;
- a general-purpose actor framework;
- a central broker task through which every message must pass;
- automatic proof of WCET correctness;
- dynamic priority assignment or EDF scheduling;
- multicore runnable migration;
- mixed-criticality partitioning or restartable protection domains;
- fully dynamic execution-mode reconfiguration.

The architecture shall not prevent these capabilities from being added later,
but the initial implementation shall remain small and statically analyzable.

## 4. Architectural drivers

The main architectural drivers are:

1. **Hard real-time behavior**: every safety-relevant execution path shall have
   bounded CPU, memory, queue, blocking, and activation behavior.
2. **Compile-time composition**: tasks, queues, resources, runnable mappings,
   and policies shall be known before the firmware starts.
3. **Generated integration**: repetitive task and port integration code shall
   be generated from reviewed configuration.
4. **Static analysis**: generated direct-call task bodies shall preserve clear
   call graphs and support stack and execution-time analysis.
5. **Component isolation**: ordinary components shall not create tasks or own
   FreeRTOS task loops.
6. **Single ownership**: mutable component state should normally be owned by
   one serialized execution lane.
7. **Explicit overload behavior**: a full queue, missed deadline, or excessive
   event burst shall result in a configured outcome rather than accidental
   blocking or silent data loss.
8. **Safety supervision**: task creation, activation, runnable execution, and
   deadline behavior shall be observable by a health-supervision mechanism.

## 5. Terminology

### 5.1 Runnable

A bounded component function that performs one unit of work. A runnable does
not contain an infinite task loop and does not decide its FreeRTOS task,
priority, or stack.

Examples:

```c
void FltMan_Tick(const FltMan_T *manager);

void SpiDev_OnRequest(
    SpiDevManager_T *manager,
    const SpiRequest_T *request);
```

### 5.2 Execution lane

A serialized execution context consisting of a FreeRTOS task plus its priority,
stack, activation sources, dispatch rules, supervision policy, and mapped
runnables.

An execution lane is a scheduling and state-ownership boundary. It is not
necessarily equivalent to one software component.

### 5.3 Periodic lane

An execution lane released from an absolute periodic schedule. One release may
call one or more run-to-completion runnables in a statically defined order.

### 5.4 Event lane

An execution lane that blocks on a statically configured queue or notification
and invokes a runnable when an event or work item becomes available.

### 5.5 Channel

A typed communication path with defined storage, capacity, delivery semantics,
producer behavior, ownership transfer, and overflow policy.

### 5.6 Mailbox

A queued channel connected to an event lane. The mailbox holds pending work
items and acts as the task's activation object.

### 5.7 Resource

A shared object requiring exclusive access when ownership cannot be confined to
one execution lane. A resource has a configured synchronization protocol,
allowed users, maximum critical-section duration, and lock-order rules.

### 5.8 Component execution contract

Machine-readable requirements supplied with a component that define the
conditions under which its runnables are correct. The contract constrains but
does not select the final task mapping.

### 5.9 Resolved execution manifest

The validated result of combining component contracts, system timing
requirements, execution mappings, platform configuration, and timing evidence.
Generated C code shall be derived from this manifest.

## 6. Design decisions

### 6.1 FreeRTOS is the direct execution backend

`Exec` and generated execution-integration code may directly use FreeRTOS APIs.
Ordinary application and middleware components shall not receive FreeRTOS task,
queue, semaphore, or mutex handles.

This decision deliberately trades operating-system portability for a smaller,
more explicit initial architecture.

### 6.2 Components expose runnables, not task loops

Components shall expose bounded runnables. `ExecCfg` shall select task mapping,
activation, priority, phase, stack, and supervision.

Component-owned task creation is prohibited unless a future architectural
decision explicitly classifies the component as a FreeRTOS infrastructure
adapter.

### 6.3 Configuration owns task and queue objects

`cfg_exec` shall own the concrete, statically allocated application execution
objects, including:

- `StaticTask_t` control blocks;
- task stack arrays;
- `StaticQueue_t` control blocks;
- queue storage arrays;
- static mutex control blocks;
- resolved task and channel descriptors;
- direct-call task wrapper functions or generated bindings.

The generic `Exec` implementation shall own common initialization,
supervision, dispatch support, and validation hooks.

### 6.4 Exec ownership does not imply a central broker task

Queue publication shall not be forwarded through a central `Exec` task.
Generated typed producer ports shall call the configured FreeRTOS primitive
directly. FreeRTOS shall make the destination task ready, and normal priority
scheduling shall select the next task.

This preserves design-time control without adding an extra scheduling hop,
broker bottleneck, or common runtime failure point.

### 6.5 Runtime registration is prohibited

Tasks, channels, work types, handlers, and resources shall be defined before
startup. The production configuration shall not register arbitrary function
pointers at runtime.

Generated task wrappers should call component runnables directly so that the
call graph remains visible to static-analysis and WCET tools.

### 6.6 Nonblocking publication is the default

Producer ports used in hard real-time paths shall default to a zero-wait queue
send. Queue saturation shall return an explicit typed result and execute the
configured overflow policy.

A blocking producer port is permitted only when its maximum blocking time and
wait dependency are declared and included in schedulability and deadlock
analysis.

### 6.7 Single-lane mutable state ownership is the default

All runnables that mutate one component instance should normally execute on the
same serialized lane. Other tasks shall interact with that instance using
typed command, event, completion, or snapshot ports.

This state-confinement rule is preferred over adding internal mutexes.

## 7. System context

```text
                                DevM
                     startup and operating modes
                                  |
                                  v
                              Exec_Init
                                  |
                  +---------------+----------------+
                  |                                |
                  v                                v
          periodic execution lanes          event execution lanes
          absolute-time releases            mailbox/notification wait
                  |                                |
                  +---------------+----------------+
                                  |
                                  v
                         component runnables

 producer runnable -> typed port -> static channel -> event lane -> handler

 cfg_exec owns concrete task, queue, stack, and resource objects.
 FreeRTOS provides scheduling, blocking, wakeup, and synchronization.
```

## 8. Ownership model

| Concern | Owner |
|---|---|
| Algorithm and state-transition semantics | Component |
| Runnable API and execution contract | Component |
| Concrete component instance and functional configuration | Component-specific configuration |
| Concrete task mapping and runnable order | `cfg_exec` |
| Period, phase, deadline, priority, stack | `cfg_exec` |
| Queue capacity and overflow policy | `cfg_exec` after validating component and system constraints |
| Static FreeRTOS task/queue/mutex storage | `cfg_exec` |
| Common startup and supervision mechanism | `Exec` |
| Task readiness and priority scheduling | FreeRTOS |
| Message interpretation | Producer/consumer component contracts |
| Message storage and ownership-transfer state | Channel implementation and configuration |
| Safe-state policy | Application safety/lifecycle management |
| Hardware watchdog decision | Independent health-supervision path |

Allocation ownership does not grant representation ownership. For example,
`cfg_exec` may allocate a component context or queue buffer, while only the
owning component or channel implementation interprets its internal layout.

## 9. Component execution contracts

Each runnable shall provide or reference an execution contract. A contract may
be generated into documentation and build metadata, but it shall be authored
alongside the component rather than inferred only from final task configuration.

### 9.1 Common contract attributes

The contract shall be able to express:

- stable component and runnable identifiers;
- activation kind;
- minimum, preferred, and maximum period where applicable;
- maximum activation gap;
- relative deadline;
- maximum permitted release jitter;
- minimum inter-arrival time for sporadic activation;
- maximum burst size and burst window;
- reentrancy and concurrency constraints;
- whether blocking, allocation, or recursion is permitted;
- callable execution contexts: task, ISR, initialization, shutdown;
- input freshness assumptions;
- event delivery semantics;
- missed-activation semantics;
- required logical resources;
- failure policy requirements;
- parameterized WCET and stack-demand information.

### 9.2 Periodic contract example

```yaml
component: FaultManager
runnable: FltMan_Tick

activation:
  kind: periodic
  preferred_period_us: 10000
  maximum_period_us: 12000
  maximum_jitter_us: 200
  maximum_skipped_activations: 0

timing:
  relative_deadline_us: 2500

execution:
  reentrant: false
  concurrent_activations: forbidden
  blocking: forbidden
  allocation: forbidden
```

If a component algorithm uses evaluation counts, the configured period becomes
part of its functional semantics. Where practical, time-dependent components
should consume an absolute timestamp or elapsed physical time so that modest
scheduling variation does not silently change their behavior.

### 9.3 Event contract example

```yaml
component: SpiDevManager
runnable: SpiDev_OnRequest

activation:
  kind: sporadic_event
  source: SPI_REQUEST_CHANNEL

arrival:
  minimum_interarrival_us: 100
  maximum_burst: 4
  burst_window_us: 1000

delivery:
  semantics: every_item
  ordering: fifo
  coalescing: forbidden

execution:
  context: task
  blocking: forbidden
  reentrant: false

timing:
  maximum_start_latency_us: 200
  relative_deadline_us: 500
```

## 10. Periodic execution model

### 10.1 Task structure

A generated periodic task body shall have the following conceptual form:

```c
static void ExecTask_Safety10ms(void *argument)
{
    TickType_t nextRelease = Exec_WaitForStartEpoch(EXEC_PHASE_SAFETY_10MS);

    for (;;)
    {
        Exec_WaitUntil(nextRelease);
        ExecMonitor_TaskRelease(EXEC_TASK_SAFETY_10MS, nextRelease);

        ExecMonitor_RunnableBegin(EXEC_RUNNABLE_FLT_MAN_TICK);
        FltMan_Tick(&g_flt_man);
        ExecMonitor_RunnableEnd(EXEC_RUNNABLE_FLT_MAN_TICK);

        ExecMonitor_TaskComplete(EXEC_TASK_SAFETY_10MS, nextRelease);
        nextRelease += EXEC_PERIOD_SAFETY_10MS;
    }
}
```

The generated production code may use `xTaskDelayUntil()` directly. It shall
use a shared startup epoch and configured phase rather than basing each task's
initial phase on the nondeterministic time at which the task first runs.

### 10.2 Runnable grouping

Multiple runnables may be mapped to one periodic lane only if their contracts
are compatible with:

- one priority and deadline;
- one stack and privilege domain;
- static sequential ordering;
- cumulative WCET;
- common overrun policy;
- no prohibited blocking;
- required data release and publication boundaries;
- safety criticality and supervision requirements.

A component shall not receive a dedicated task merely because it is a separate
component.

### 10.3 Missed release and deadline policy

The configuration shall distinguish:

- late task start;
- runnable WCET-budget violation;
- task deadline miss;
- next release already in the past;
- skipped activation;
- repeated overload.

The response shall be selected explicitly. Permitted policies may include:

- record and continue;
- skip obsolete work;
- execute one bounded recovery activation;
- change to a prevalidated degraded schedule;
- request safe state;
- stop servicing the hardware watchdog.

An unbounded catch-up loop is prohibited.

## 11. Event and queue execution model

### 11.1 Consumer interface

An event-driven component shall expose a bounded handler rather than a queue
receive loop:

```c
void Consumer_OnItem(
    Consumer_T *consumer,
    const WorkItem_T *item);
```

The consumer shall not receive a `QueueHandle_t`.

### 11.2 Producer interface

A producer shall call a generated typed port:

```c
typedef enum
{
    WORK_PORT_ACCEPTED,
    WORK_PORT_FULL,
    WORK_PORT_CLOSED,
    WORK_PORT_WRONG_MODE
} WorkPort_Result_T;

WorkPort_Result_T WorkPort_TryPublish(const WorkItem_T *item);
```

The generated port implementation may call `xQueueSendToBack()` with zero wait
time. It shall translate FreeRTOS return values into the semantic port result
and execute any configured failure-latching behavior.

### 11.3 Consumer task structure

```c
static void ExecTask_WorkConsumer(void *argument)
{
    WorkItem_T item;

    for (;;)
    {
        const BaseType_t received = xQueueReceive(
            g_work_queue,
            &item,
            portMAX_DELAY);

        if (received != pdPASS)
        {
            Exec_FailureLatch(EXEC_FAILURE_WORK_QUEUE_RECEIVE);
            continue;
        }

        ExecMonitor_RunnableBegin(EXEC_RUNNABLE_CONSUMER_ON_ITEM);
        Consumer_OnItem(&g_consumer, &item);
        ExecMonitor_RunnableEnd(EXEC_RUNNABLE_CONSUMER_ON_ITEM);
    }
}
```

FreeRTOS queue receive blocking is the intended task-suspension mechanism. No
CPU time is consumed while the consumer waits for an empty queue to become
nonempty.

If publication makes a higher-priority consumer ready, FreeRTOS may preempt the
producer before the queue-send API returns. This scheduling behavior shall be
considered when assigning producer and consumer priorities.

### 11.4 Delivery semantics

Every channel shall select one delivery model:

| Model | Meaning | Typical use |
|---|---|---|
| Every item | Every accepted item is processed once | transaction requests, buffer completions |
| Latest value | Intermediate updates may be replaced | measurements, status snapshots |
| Counting | Occurrences are counted without payload duplication | releases, completion counts |
| Latched level | Condition remains pending until acknowledged | faults, shutdown requests |
| Priority ordered | Urgent accepted items precede normal items | carefully bounded service requests |

Triple buffering is a latest-value mechanism. It shall not be used alone when
every intermediate edge or transaction completion must be retained.

### 11.5 Copy and zero-copy channels

Small control messages should normally be copied by value into a FreeRTOS
queue. This has simple lifetime and ownership semantics.

Large payloads may use zero-copy handles. In that case, the queue shall contain
a fixed-size descriptor or buffer handle, and the channel contract shall define
the complete ownership state machine:

```text
FREE -> RESERVED_BY_PRODUCER -> PUBLISHED -> OWNED_BY_CONSUMER -> FREE
```

The producer shall not modify a published buffer. The consumer shall release
each accepted buffer exactly once. Stale, duplicated, or invalid-generation
handles shall be detectable.

### 11.6 Queue capacity and overflow

Queue capacity shall be derived from a declared arrival and service model. It
shall not be selected only by observation during nominal testing.

Each queue shall define an overflow policy. Supported policies may include:

- reject newest and return `FULL`;
- overwrite latest value for a latest-value channel;
- latch an overflow fault;
- request degraded or safe state;
- reject lower-priority work while preserving reserved safety capacity.

Silent loss is prohibited unless the channel contract explicitly states that
loss is harmless and describes how the consumer observes the latest valid
state.

### 11.7 Dispatch batching and budget

A continuously nonempty queue can keep its consumer permanently ready. Every
work-server configuration shall therefore define its dispatch rule:

- one item per dispatch iteration;
- drain all pending work;
- maximum number of items per batch;
- maximum CPU budget per activation or replenishment interval.

Safety-relevant work servers shall use a bounded item count or execution budget.
The selected model shall be included in response-time analysis.

### 11.8 Multiple event sources

When one execution lane consumes multiple event types, the configuration shall
select one deterministic design:

- one tagged-union queue;
- FreeRTOS queue set plus explicit dispatch precedence;
- task-notification bits combined with typed storage channels;
- separate queues polled in a generated, fixed priority order after wakeup.

Implicit dependency on whichever FreeRTOS object happens to be inspected first
is prohibited. Dispatch precedence shall be part of configuration.

## 12. Work-server model

A work server is an event lane that processes one of a statically admitted set
of work types.

### 12.1 Static admission

The allowed work types shall be known at build time:

```yaml
server: RealtimeIoServer
priority: 7
queue_capacity: 8
maximum_batch: 4
budget_us: 400
budget_period_us: 1000

work_types:
  - Spi_ProcessCompletion
  - Adc_ProcessFrame
  - Gpio_ProcessCapture
```

Submitting an arbitrary runtime function pointer is prohibited for production
hard-real-time work servers.

### 12.2 Work-server separation

Separate work servers shall be used when work items have incompatible:

- criticality;
- priority or deadline;
- privilege or MPU domain;
- blocking behavior;
- execution budget;
- overflow policy;
- watchdog or recovery policy.

A shared best-effort server may be used for logging, statistics, cleanup, and
noncritical diagnostics. Hard-deadline work shall not be submitted to a
best-effort queue.

## 13. Mutual exclusion and resource management

### 13.1 Preferred order

The following mechanisms shall be considered in order:

1. Confine mutable state to one execution lane.
2. Publish immutable or triple-buffered snapshots to readers.
3. Use atomics for small flags, counters, indexes, or publication tokens.
4. Apply a generated whole-runnable resource guard.
5. Inject a narrowly scoped configured mutex/resource capability.
6. Allow synchronous blocking access only with explicit analysis.

### 13.2 State confinement

If `SpiDev_OnRequest`, `SpiDev_OnDmaComplete`, and `SpiDev_OnTimeout` all mutate
one `SpiDevManager_T`, they should normally be mapped to one execution lane.
Clients submit requests through typed ports and never receive a mutable manager
pointer.

This removes the need for a mutex inside `SpiDevManager`.

### 13.3 Generated whole-runnable guard

A runnable may declare exclusive use of a logical resource:

```yaml
runnable: Calibration_Update
exclusive_resources:
  - CalibrationDatabase
```

The generator may produce:

```c
const BaseType_t acquired = xSemaphoreTake(
    g_calibration_mutex,
    EXEC_CALIBRATION_LOCK_TIMEOUT_TICKS);

if (acquired == pdPASS)
{
    Calibration_Update(&g_calibration);

    if (xSemaphoreGive(g_calibration_mutex) != pdPASS)
    {
        Exec_FailureLatch(EXEC_FAILURE_RESOURCE_RELEASE);
    }
}
else
{
    Exec_FailureLatch(EXEC_FAILURE_RESOURCE_TIMEOUT);
}
```

This approach is permitted only when the complete runnable forms a short,
bounded critical section and cannot block or call another protected resource.

### 13.4 Narrow resource capability

When only a subsection requires protection, the component may receive an
opaque logical resource reference whose implementation is owned by `cfg_exec`.
The component shall not receive the underlying `SemaphoreHandle_t`.

The capability API shall define:

- task-only or ISR-allowed context;
- blocking or try-lock behavior;
- maximum timeout;
- recursion policy;
- configured users;
- lock order;
- failure result.

### 13.5 FreeRTOS mutex rules

Where a task mutex is necessary:

- use `xSemaphoreCreateMutexStatic()`;
- do not substitute a binary semaphore for a mutex;
- include priority-inheritance blocking in response-time analysis;
- define a global lock order;
- prohibit undeclared nested locking;
- do not acquire a task mutex from an ISR;
- do not hold a mutex while waiting for a queue, notification, DMA completion,
  timer, or another task;
- define and monitor a maximum critical-section duration;
- check every take/give result that can fail.

FreeRTOS priority inheritance reduces but does not remove the need for blocking
and deadlock analysis. A future revision may introduce a generated
priority-ceiling resource service.

### 13.6 Task/ISR shared state

Task/ISR communication shall use an ISR-safe queue, task notification, bounded
interrupt-masked critical section, or verified lock-free publication scheme.
Task mutexes shall not be used from ISR context.

Maximum interrupt masking duration and permitted interrupt priorities shall be
documented and included in latency analysis.

## 14. SPI device-manager application of the model

The preferred SPI architecture is a single-owner active server:

```text
clients -> typed SPI request channel -> SPI execution lane
                                           |
                                           v
                                  SPI registers and DMA
                                           |
                                 DMA completion ISR
                                           |
                                           v
                               completion notification
                                           |
                                           v
                               SPI completion runnable
                                           |
                                           v
                               typed client completion port
```

The SPI manager owns:

- SPI registers;
- chip-select sequencing;
- DMA setup and completion state;
- transaction timeout state;
- transaction scheduling policy.

Clients shall not lock a public SPI mutex or access SPI registers. They submit
statically allocated requests or buffer handles through typed ports.

Once a transaction starts, a later urgent request may be blocked by the current
nonpreemptible transfer. The maximum duration of a lower-priority transaction
shall therefore contribute to the blocking bound of urgent SPI traffic.

Possible mitigations include transfer-size limits, protocol-safe chunking,
priority queues, reserved service slots, or assignment of critical devices to a
separate peripheral.

## 15. FreeRTOS object policy

### 15.1 Tasks

- Application tasks shall use `xTaskCreateStatic()`.
- Task control blocks and stacks shall be supplied by `cfg_exec`.
- Every task shall have an explicit stack size.
- `configMINIMAL_STACK_SIZE` shall not be used as evidence that a task stack is
  sufficient.
- Every creation result shall be checked before scheduler start.
- Application task deletion is prohibited during normal operation.

### 15.2 Queues

- Application queues shall use `xQueueCreateStatic()`.
- Queue length and element size shall be generated constants.
- Queue storage shall be statically aligned and sized.
- Task producers shall use task-context queue APIs.
- ISR producers shall use only `FromISR` APIs and configured legal interrupt
  priorities.
- Blocking send shall be prohibited by default.

### 15.3 Task notifications

Task notifications may be used when:

- payload data is stored elsewhere;
- one task is the unique notification consumer;
- counting, bit-set, or latest-value semantics match the contract;
- the configuration accounts for notification-value saturation or overwrite.

### 15.4 Software timers

FreeRTOS software-timer callbacks shall not directly execute safety-critical or
long-running component work. They may perform bounded activation signaling when
the shared timer-daemon task's latency and priority are acceptable and analyzed.

## 16. Initialization and lifecycle

### 16.1 Startup sequence

The intended startup sequence is:

1. `DevM` initializes hardware and memory infrastructure.
2. Component-specific configuration objects are initialized if required.
3. `Exec_Init()` validates resolved configuration assumptions available at
   runtime.
4. `Exec_Init()` creates every static queue, mutex, and task.
5. Every creation result is collected and checked.
6. Tasks remain behind a startup barrier or wait for a common epoch.
7. `DevM` starts FreeRTOS only if all required objects were created.
8. `DevM` releases the application start barrier and establishes the common
   scheduling epoch.

Startup failure shall not be reported only through a queue or task that may
have failed to initialize.

### 16.2 Operating modes

The initial revision may implement only one normal execution mode. Future modes
shall use complete, prevalidated schedules such as:

- initialization;
- normal operation;
- degraded operation;
- safe state;
- shutdown.

Mode transitions shall occur at defined scheduling boundaries. Arbitrary
runtime mutation of task descriptors is prohibited.

## 17. Failure model

| Failure | Detection | Required configured response |
|---|---|---|
| Invalid generated configuration | Generator/build validator | Reject build |
| Static object creation failure | `Exec_Init()` | Abort scheduler startup or enter startup-safe path |
| Producer publishes to full queue | Typed port result | Reject/overwrite/latch fault according to channel policy |
| Event burst exceeds declared model | Queue watermark or overflow | Latch contract violation; apply channel safety policy |
| Runnable budget exceeded | Runtime execution monitor | Record, supervise, degrade, or request safe state |
| Task deadline missed | Task completion monitor | Apply configured deadline-miss policy |
| Expected activation absent | Independent alive supervision | Degrade or request safe state |
| Mutex acquisition timeout | Resource API result | Abort operation and apply resource-specific policy |
| Invalid zero-copy handle | Generation/state validation | Reject handle and latch integrity fault |
| Work-server budget exhausted | Dispatch budget monitor | Suspend service until configured replenishment or escalate |

Failure reporting shall avoid circular dependency. In particular, an execution
failure shall be latched in independently accessible state before asynchronous
diagnostic publication is attempted.

## 18. Configuration sources and generation

The target generation pipeline is:

```text
component.contract.yaml
system.requirements.yaml
execution.mapping.yaml
freertos.platform.yaml
timing_evidence.yaml
           |
           v
 configuration resolver
           |
           v
 deterministic validators
           |
           v
 resolved_execution_manifest.json
           |
           +--> generated cfg_exec C/H
           +--> generated typed ports
           +--> generated direct-call task bodies
           +--> generated static assertions
           +--> schedulability-analysis input/report
           +--> traceability report
           +--> runtime supervision tables
```

The resolved manifest is generated and shall not be manually edited.

## 19. Configuration validation

The build shall eventually perform at least the following deterministic checks:

1. Schema, unit, range, and stable-identifier validation.
2. Unique task, runnable, channel, and resource identifiers.
3. All referenced runnables and component instances exist.
4. Selected periods satisfy component timing contracts.
5. Period values are representable by the configured FreeRTOS tick or approved
   higher-resolution activation source.
6. Deadlines do not exceed permitted contract bounds.
7. Blocking runnables are not mapped to basic run-to-completion periodic lanes.
8. Non-reentrant runnables cannot execute concurrently.
9. Mutable component instances have exactly one execution-lane owner unless an
   explicit resource model authorizes sharing.
10. Queue capacity is consistent with declared burst and service assumptions.
11. Every queue has an overflow policy.
12. Zero-copy channels have complete ownership transitions.
13. Work-server admission and dispatch budgets are bounded.
14. Resource users and lock orders are complete and acyclic.
15. Resource blocking is included in task response-time analysis.
16. Periodic and sporadic tasks pass schedulability analysis.
17. ISR execution and release interference are included.
18. Task stack and static memory allocations fit configured memory regions.
19. MPU/privilege access is compatible with runnable and object placement.
20. Every supervised task has start, completion, and alive checkpoints.

C `_Static_assert` checks shall complement these validators but are not a
replacement for graph, timing, capacity, or resource analysis.

## 20. AI-assisted validation policy

LLM agents may assist the design-time workflow by:

- detecting missing or suspicious constraints;
- reviewing component contracts against system mappings;
- proposing edge cases and stress tests;
- explaining validator counterexamples;
- suggesting repairs to invalid configurations;
- reviewing failure and overload policies.

An LLM result shall be advisory. It shall not directly authorize a production
configuration. Any LLM-proposed configuration change must pass the same
deterministic schema, graph, schedulability, capacity, and safety validators as
a human-authored change.

LLM agents are therefore outside the trusted configuration acceptance path.

## 21. Runtime supervision

The generated configuration should provide identifiers and thresholds for:

- task release checkpoints;
- runnable start and completion checkpoints;
- task deadline checks;
- alive counters;
- logical execution order;
- queue high-water marks and overflow counters;
- work-server budget use;
- mutex maximum hold time where measurable;
- stack high-water observations during development.

The supervised component shall not be the only mechanism responsible for
detecting its own starvation. An independent health supervisor and hardware
watchdog path shall consume the generated checkpoints.

## 22. CMake target architecture

The existing target split is retained and refined:

### 22.1 `Exec_api`

Public lifecycle and status API. It shall expose no concrete FreeRTOS object
layout to ordinary components.

Candidate API:

```c
typedef struct Exec_Config_Tag Exec_Config_T;

typedef enum
{
    EXEC_INIT_OK,
    EXEC_INIT_INVALID_CONFIG,
    EXEC_INIT_OBJECT_CREATION_FAILED
} Exec_InitResult_T;

Exec_InitResult_T Exec_Init(const Exec_Config_T *config);
void Exec_ReleaseStartBarrier(void);
```

The current `void Exec_Init(void)` may be retained temporarily, but an explicit
configuration handle and result are preferred so startup failure cannot be
ignored.

### 22.2 `Exec_cfg`

Configuration-facing types required to construct the resolved execution
configuration. This target may include FreeRTOS-specific static-object types in
the initial architecture, but they shall not propagate into component public
APIs.

### 22.3 `Exec`

Common implementation of:

- initialization;
- start barrier and epoch handling;
- monitoring hooks;
- common periodic/event dispatch helpers;
- configuration consistency checks possible at runtime;
- status and failure latching.

This target may privately link FreeRTOS.

### 22.4 `cfg_exec`

Application-specific, eventually generated configuration containing:

- static task, stack, queue, and resource storage;
- task and channel descriptors;
- task-to-runnable mappings;
- direct-call generated task wrappers;
- concrete periods, phases, priorities, deadlines, and budgets;
- typed producer-port implementations or bindings;
- compile-time assertions.

`cfg_exec` may privately include component runnable APIs and FreeRTOS headers.
Those dependencies shall not propagate to producer or consumer component public
targets.

## 23. Public API separation

Where useful, a component should expose separate build targets or include
facades:

```text
component_client_api
    commands, events, message types, typed producer ports

component_runnable_api
    runnable functions callable only by generated execution integration

component_cfg_types
    concrete layout needed only by component configuration

component_internal
    private implementation
```

This prevents a client task from directly invoking a non-reentrant mutable
runnable that is intended to execute only on its owner lane.

## 24. Verification strategy

### 24.1 Component unit tests

Runnables shall be testable as ordinary C functions without starting FreeRTOS.
Tests shall cover functional behavior, contract boundary conditions, invalid
inputs, state transitions, and configured maximum sizes.

### 24.2 Exec unit tests

Where practicable, test:

- configuration validation;
- result translation;
- failure latching;
- dispatch budget calculations;
- tick-wrap-safe time comparisons;
- missed-release policy decisions.

### 24.3 FreeRTOS integration tests

Integration tests shall cover:

- static task and queue creation;
- shared-epoch periodic release;
- higher-priority consumer wakeup;
- nonblocking producer behavior on full queue;
- queue burst handling;
- ISR-to-task activation;
- deadline and execution-budget violations;
- mutex priority-inheritance behavior where used;
- startup failure behavior;
- task and queue supervision under sustained load.

### 24.4 Analysis evidence

The build or release process shall retain:

- resolved configuration manifest and hash;
- generated source version;
- response-time analysis report;
- queue-capacity report;
- resource and wait-for graph report;
- stack analysis or justified measured bounds;
- static-analysis results;
- test results linked to configuration identifiers.

## 25. Initial implementation sequence

1. Define `Exec_Config_T`, explicit `Exec_InitResult_T`, and a static task
   descriptor suitable for FreeRTOS.
2. Move FaultManager periodic-task ownership from `test_swc` into `cfg_exec`.
3. Create the first statically allocated periodic lane with shared-epoch
   release and deadline monitoring.
4. Define a typed static mailbox and generated-style event consumer wrapper.
5. Implement nonblocking typed producer-port result handling.
6. Add static queue storage, capacity, and overflow policy to `cfg_exec`.
7. Define the execution contract format and manually validate the first
   contracts before automating generation.
8. Add generation of direct-call task bodies and typed ports.
9. Add deterministic configuration, queue, resource, and schedulability
   validators.
10. Add independent alive/deadline/logical supervision integration.

## 26. Deferred decisions

The following require later architecture decisions:

- exact source format and schema language for component contracts;
- exact queue-capacity analysis method for bursty producers;
- priority-ceiling implementation versus FreeRTOS priority inheritance;
- support for synchronous request/reply ports;
- mode-specific task creation versus persistent tasks behind mode gates;
- high-resolution timer activation below the FreeRTOS tick period;
- MPU placement and privilege boundaries;
- multicore and accelerator interference models;
- qualification strategy for generators and deterministic validators;
- criteria for admitting an LLM-assisted review into release evidence.

## 27. References

1. AUTOSAR, *Specification of RTE Software*, Classic Platform R24-11:  
   <https://www.autosar.org/fileadmin/standards/R24-11/CP/AUTOSAR_CP_SWS_RTE.pdf>
2. AUTOSAR, *Specification of Timing Extensions for Classic Platform*, R23-11:  
   <https://www.autosar.org/fileadmin/standards/R23-11/CP/AUTOSAR_CP_TPS_TimingExtensions.pdf>
3. AUTOSAR, *Timing Analysis and Design*, Foundation R25-11:  
   <https://www.autosar.org/fileadmin/standards/R25-11/FO/AUTOSAR_FO_TR_TimingAnalysis.pdf>
4. FreeRTOS, *Queues*:  
   <https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/02-Queues-mutexes-and-semaphores/01-Queues>
5. FreeRTOS, *xTaskDelayUntil*:  
   <https://www.freertos.org/Documentation/02-Kernel/04-API-references/02-Task-control/03-xTaskDelayUntil>
6. FreeRTOS, *Task creation*:  
   <https://www.freertos.org/Documentation/02-Kernel/04-API-references/01-Task-creation/01-xTaskCreate>
7. Henzinger, Horowitz, and Kirsch, *Giotto: a Time-triggered Language for
   Embedded Programming*:  
   <https://www2.eecs.berkeley.edu/Pubs/TechRpts/2000/6436.html>
8. Sprunt, Sha, and Lehoczky, *Scheduling Sporadic and Aperiodic Events in a
   Hard Real-Time System*:  
   <https://www.sei.cmu.edu/library/scheduling-sporadic-and-aperiodic-events-in-a-hard-real-time-system/>
9. Sha, Rajkumar, and Lehoczky, *Priority Inheritance Protocols: An Approach to
   Real-Time Synchronization*:  
   <https://research.ibm.com/publications/priority-inheritance-protocols-an-approach-to-real-time-synchronization>
