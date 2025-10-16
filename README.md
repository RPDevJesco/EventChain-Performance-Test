# EventChains: A Universal Design Pattern

## Executive Summary

EventChains is a design pattern for building sequential workflows with composable middleware. It combines the clarity of procedural programming, the composability of functional programming, and the flexibility of object-oriented design into a single, language-agnostic pattern that works across paradigms and platforms.

---

## What is EventChains?

EventChains is a design pattern that orchestrates sequential execution of discrete units of work (events) through a configurable pipeline of cross-cutting concerns (middleware). It provides a structured approach to building workflows where:

1. **Events** represent individual steps in a process
2. **Context** carries shared state between events
3. **Middleware** adds reusable behaviors around event execution
4. **Chain** orchestrates the sequential flow

### Core Components

**Event Context**
- A shared data container that flows through the entire chain
- Enables communication between sequential events
- Acts as a type-safe (or dynamically-typed) dictionary
- Persists for the lifetime of a single chain execution

**Chainable Event**
- A discrete unit of business logic
- Receives context and can read/modify it
- Returns a result indicating success or failure
- Should be stateless and testable in isolation

**Event Chain**
- Orchestrates sequential execution of events
- Manages middleware pipeline construction
- Handles error propagation based on fault tolerance mode
- Provides lifecycle management for the entire workflow

**Middleware**
- Wraps event execution with cross-cutting concerns
- Executes in reverse order of registration (LIFO)
- Can run logic before, after, or around events
- Examples: logging, timing, error handling, transactions, validation

---

## Who Should Use EventChains?

### Primary Audiences

**Backend Developers**
- Building API request/response pipelines
- Creating order processing workflows
- Implementing business process automation
- Developing event-driven architectures

**Systems Architects**
- Designing modular, maintainable systems
- Separating cross-cutting concerns from business logic
- Building frameworks that others will extend
- Creating testable, composable architectures

**Game Developers**
- Implementing AI behavior trees
- Building game state machines
- Creating combat or spell effect chains
- Managing turn-based game logic

**DevOps Engineers**
- Building deployment pipelines
- Creating infrastructure automation workflows
- Implementing CI/CD processes with multiple stages

**Full-Stack Developers**
- Processing HTTP requests through multiple validation/transformation stages
- Building data processing pipelines
- Implementing multi-step form submissions

### When NOT to Use EventChains

- **Simple CRUD operations** with no intermediate processing
- **Parallel workflows** where steps don't depend on previous results
- **Real-time streaming** where buffering sequential steps adds latency
- **Fire-and-forget scenarios** with no need for ordered execution or result tracking

---

## When to Use EventChains?

### Ideal Scenarios

**Sequential Processing is Required**
- Each step depends on the results of previous steps
- Order of operations matters for correctness
- Data transformation flows through multiple stages

**Multiple Cross-Cutting Concerns Exist**
- You need logging, timing, validation, error handling across multiple operations
- The same concerns apply to different workflows
- You want to avoid duplicating infrastructure code

**Workflows Need to be Composable**
- Different use cases share some but not all steps
- You want to mix and match events to create new workflows
- Reusability and maintainability are priorities

**Testing Isolation is Important**
- Each step should be unit-testable independently
- You want to mock/stub individual steps without affecting others
- Integration testing should be straightforward

**Business Logic Changes Frequently**
- Workflows evolve and steps get added/removed/reordered
- You need clear separation between infrastructure and business logic
- Multiple teams work on different parts of the workflow

### Real-World Examples

**E-Commerce Order Processing**
```
ValidateOrder → CalculateTotals → CheckInventory → 
ApplyDiscounts → ProcessPayment → CreateShipment → 
SendConfirmation → UpdateAnalytics
```

**HTTP Request Processing**
```
ParseRequest → Authenticate → Authorize → 
ValidateInput → RouteToHandler → ExecuteBusinessLogic → 
FormatResponse → LogRequest
```

**Game AI Decision Making**
```
PerceiveEnvironment → EvaluateThreat → 
SelectStrategy → CalculateMovement → 
ExecuteAction → UpdateState
```

**Data ETL Pipeline**
```
ExtractFromSource → ValidateSchema → 
TransformData → EnrichWithMetadata → 
ValidateBusinessRules → LoadToDestination → 
UpdateCatalog
```

---

## Where Can EventChains Be Applied?

### Technology Domains

**Web Development**
- REST API middleware pipelines
- GraphQL resolver chains
- WebSocket message processing
- Server-side rendering pipelines

**Enterprise Applications**
- Business process management
- Workflow automation
- Order management systems
- Payment processing

**Game Development**
- Behavior trees for AI
- Spell/ability effect chains
- Turn-based combat systems
- Save/load game state

**Data Engineering**
- ETL pipelines
- Stream processing stages
- Data validation workflows
- Report generation

**DevOps & Infrastructure**
- Deployment pipelines
- Configuration management
- Health check sequences
- Backup/restore workflows

**IoT & Embedded Systems**
- Sensor data processing chains
- Command execution pipelines
- State machine implementations
- Event-driven control systems

### Language Implementations

The pattern has been successfully implemented in:
- **C#** (.NET, object-oriented, async/await)
- **Ruby** (dynamic, duck-typed, blocks)
- **Java** (enterprise, strongly-typed, functional interfaces)
- **C** (procedural, function pointers, manual memory management)
- **JavaScript/TypeScript** (prototype-based, promises, async)

The pattern adapts idiomatically to each language's strengths while maintaining core concepts.

---

## Why Use EventChains?

### Core Benefits

**1. Separation of Concerns**
- Business logic stays pure and focused
- Infrastructure code (logging, timing, error handling) lives in middleware
- Events don't know or care about cross-cutting concerns
- Each piece has a single, clear responsibility

**2. Composability and Reusability**
- Events can be reused across different chains
- Middleware applies universally to any event
- Mix and match components to create new workflows
- Build libraries of common operations

**3. Testability**
- Events can be unit tested in complete isolation
- Mock the context to test specific scenarios
- Test middleware independently of events
- Integration testing follows the natural workflow structure

**4. Maintainability**
- Clear, linear flow is easy to understand
- Changes to middleware don't affect events
- Adding/removing/reordering events is straightforward
- Self-documenting through declarative chain composition

**5. Flexibility**
- Support multiple fault tolerance modes (strict, lenient, best-effort, custom)
- Adapt to different error handling strategies
- Scale from simple sequences to complex workflows
- Works with synchronous or asynchronous execution

**6. Cross-Paradigm Compatibility**
- Procedural clarity in execution flow
- Functional composability in design
- Object-oriented flexibility in implementation
- Works naturally in any programming paradigm

### Comparison to Alternative Patterns

**vs. Chain of Responsibility**
- EventChains: Fixed, known sequence of handlers
- CoR: Dynamic selection, each handler decides if it handles the request
- EventChains better for: Predictable workflows with all steps known
- CoR better for: Dynamic dispatch where appropriate handler is determined at runtime

**vs. Pipeline Pattern**
- EventChains: Built-in middleware support, error handling, context management
- Pipeline: Simpler, more focused on transformation
- EventChains better for: Complex workflows with cross-cutting concerns
- Pipeline better for: Simple data transformations

**vs. Saga Pattern**
- EventChains: Synchronous sequential execution within a single process
- Saga: Distributed transactions across services with compensation
- EventChains better for: In-process workflows
- Saga better for: Distributed systems with eventual consistency

**vs. Mediator Pattern**
- EventChains: Linear sequence of operations
- Mediator: Central coordinator for complex object interactions
- EventChains better for: Sequential workflows
- Mediator better for: Coordinating multiple independent objects

---

## How Does EventChains Work?

### Conceptual Model

Think of EventChains like an assembly line:

1. **Raw Material** = Initial context data
2. **Stations** = Events that process the material
3. **Inspectors** = Middleware that check quality at each station
4. **Conveyor Belt** = The chain that moves material between stations
5. **Final Product** = Resulting context after all processing

### Execution Flow

```
User calls Execute()
    ↓
Chain constructs middleware pipeline (reverse order)
    ↓
For each event:
    ↓
    Outermost middleware wraps next layer
        ↓
        Next middleware wraps next layer
            ↓
            ... (more middleware layers)
                ↓
                Core event executes
                ↓
            ... (middleware post-processing)
        ↓
    Middleware post-processing
    ↓
Continue to next event or stop on error
    ↓
Return final result with context and any failures
```

### Middleware Execution Order (The "Gift Wrapping" Analogy)

When you wrap a gift:
- First you put it in a **box** (innermost layer)
- Then you wrap it with **paper** (middle layer)  
- Finally you add a **bow** (outermost layer)

When someone unwraps it:
- First they remove the **bow** (outermost)
- Then they tear off the **paper** (middle)
- Finally they open the **box** (innermost)

Middleware works the same way:

**Registration Order:**
1. LoggingMiddleware
2. TimingMiddleware  
3. ErrorHandlingMiddleware

**Execution Order:**
```
ErrorHandlingMiddleware (outermost) {
    try {
        TimingMiddleware {
            start timer
            LoggingMiddleware {
                log "Starting event"
                → ACTUAL EVENT EXECUTES HERE ←
                log "Event completed"
            }
            stop timer, log duration
        }
    } catch (error) {
        handle error
    }
}
```

This ensures that:
- Error handlers wrap everything and catch all exceptions
- Timers measure the complete execution time
- Loggers see the full context of what happened

### The Context: Data Flow Between Events

Context is the shared memory that flows through the chain:

```
Event 1: Validate Order
  - Reads: order_id, items
  - Writes: validated=true, customer_id

Event 2: Calculate Totals
  - Reads: items
  - Writes: subtotal, tax, total

Event 3: Process Payment
  - Reads: total, customer_id
  - Writes: payment_id, payment_status

Event 4: Send Confirmation
  - Reads: customer_id, payment_id, total
  - Writes: email_sent=true
```

Each event adds information that later events can use.

### Fault Tolerance Modes

**STRICT Mode**
- Any event failure stops the chain immediately
- Use for: Critical workflows where partial completion is unacceptable
- Example: Financial transactions

**LENIENT Mode**
- Non-critical failures are logged but chain continues
- Use for: Workflows where some steps are optional
- Example: Analytics tracking in an order process

**BEST_EFFORT Mode**
- All events are attempted, failures collected
- Use for: Fire-and-forget operations
- Example: Batch notification sending

**CUSTOM Mode**
- User-defined logic determines whether to continue after each failure
- Use for: Complex scenarios with nuanced error handling
- Example: Multi-tenant processing where some tenant failures are acceptable

### Implementation Patterns

**Pattern 1: Functional Style**
```
Events are pure functions
Context is immutable
Each event returns a new context
Middleware composes functions
```

**Pattern 2: Object-Oriented Style**
```
Events are classes implementing IChainableEvent
Context is a mutable object
Events modify context in place
Middleware wraps event execution
```

**Pattern 3: Procedural Style**
```
Events are function pointers
Context is a struct or dictionary
Events operate on shared state
Middleware uses callbacks
```

All three patterns achieve the same conceptual model while respecting language idioms.

---

## How to Implement EventChains

### Universal Implementation Steps

**Step 1: Define Your Core Abstractions**

Regardless of language, you need:

1. **Context container** for shared state
   - Dictionary/map/hash for key-value storage
   - Type safety mechanism (generics, templates, or dynamic typing)
   
2. **Event interface/contract**
   - Method to execute with context
   - Return success/failure indicator
   
3. **Middleware interface/contract**
   - Wraps event execution
   - Receives next-in-chain callback
   
4. **Chain orchestrator**
   - Holds events and middleware
   - Manages execution order
   - Handles errors per tolerance mode

**Step 2: Implement Context Management**

```
// Pseudocode
class EventContext:
    private data = empty_dictionary
    
    function set(key, value):
        data[key] = value
    
    function get(key):
        return data[key]
    
    function has(key):
        return key exists in data
```

**Step 3: Define Event Contract**

```
// Pseudocode
interface ChainableEvent:
    function execute(context) returns Result
```

**Step 4: Define Middleware Contract**

```
// Pseudocode
interface Middleware:
    function execute(event, context, next) returns Result
```

**Step 5: Implement Chain Orchestration**

```
// Pseudocode
class EventChain:
    private events = []
    private middlewares = []
    private context = new EventContext()
    
    function add_event(event):
        events.append(event)
    
    function use_middleware(middleware):
        middlewares.append(middleware)
    
    function execute():
        // Build pipeline (reverse middleware order)
        pipeline = base_execution_function
        
        for middleware in reverse(middlewares):
            pipeline = middleware.wrap(pipeline)
        
        // Execute each event through pipeline
        for event in events:
            result = pipeline(event, context)
            
            if result.failed and should_stop():
                return failure_result
        
        return success_result
```

### Language-Specific Considerations

**Statically-Typed Languages (C#, Java, TypeScript)**
- Use generics for type-safe context
- Define clear interfaces for events and middleware
- Leverage async/await for asynchronous chains
- Use builder pattern for fluent chain construction

**Dynamically-Typed Languages (Ruby, Python, JavaScript)**
- Use duck typing for flexible event contracts
- Leverage closures for middleware composition
- Use blocks/lambdas for inline event definition
- Embrace dynamic context access

**Systems Languages (C, C++, Rust)**
- Use function pointers or traits
- Manual memory management for context
- Zero-cost abstractions where possible
- Consider compile-time pipeline optimization

**Functional Languages (Haskell, F#, Elixir)**
- Events as pure functions
- Immutable context with monadic composition
- Middleware as function transformers
- Leverage pattern matching for error handling

### Testing Strategies

**Unit Testing Events**
```
test "ValidateOrderEvent validates items":
    context = create_context()
    context.set("items", [valid_item])
    
    event = new ValidateOrderEvent()
    result = event.execute(context)
    
    assert result.success
    assert context.get("validated") == true
```

**Integration Testing Chains**
```
test "Order processing completes successfully":
    chain = new EventChain()
    chain.add_event(new ValidateOrderEvent())
    chain.add_event(new ProcessPaymentEvent())
    
    result = chain.execute()
    
    assert result.success
    assert context.has("payment_id")
```

**Testing Middleware**
```
test "TimingMiddleware records duration":
    middleware = new TimingMiddleware()
    mock_event = create_mock_event()
    
    result = middleware.execute(mock_event, context, next)
    
    assert context.has("duration_ms")
```

---

## Advanced Patterns and Techniques

### Conditional Event Execution

Skip events based on context state:

```
class ConditionalEvent implements ChainableEvent:
    private condition_key
    private inner_event
    
    function execute(context):
        if context.get(condition_key):
            return inner_event.execute(context)
        return success() // Skip
```

### Branching Chains

Execute different sub-chains based on conditions:

```
class BranchingEvent implements ChainableEvent:
    function execute(context):
        if context.get("user_type") == "premium":
            return premium_chain.execute(context)
        else:
            return standard_chain.execute(context)
```

### Parallel Event Execution Within a Step

While the chain is sequential, a single event can spawn parallel work:

```
class ParallelNotificationEvent implements ChainableEvent:
    function execute(context):
        tasks = [
            send_email_async(context),
            send_sms_async(context),
            send_push_notification_async(context)
        ]
        
        wait_for_all(tasks)
        return success()
```

### Event Composition

Combine multiple events into a single reusable unit:

```
class CompositeEvent implements ChainableEvent:
    private sub_events = []
    
    function execute(context):
        for event in sub_events:
            result = event.execute(context)
            if result.failed:
                return result
        return success()
```

### Retry Logic in Middleware

```
class RetryMiddleware implements Middleware:
    private max_attempts = 3
    
    function execute(event, context, next):
        for attempt in 1 to max_attempts:
            result = next(event, context)
            if result.success:
                return result
            
            if attempt < max_attempts:
                wait(exponential_backoff(attempt))
        
        return result // Final failure
```

---

## Best Practices

### Event Design

**DO:**
- Keep events focused on a single responsibility
- Make events stateless (all state in context)
- Return clear success/failure indicators
- Use descriptive event names (VerbNounEvent)

**DON'T:**
- Store state in event instances
- Make events depend on execution order (use context)
- Throw unhandled exceptions from events
- Mix business logic with infrastructure concerns

### Middleware Design

**DO:**
- Make middleware reusable across different chains
- Handle errors gracefully
- Use descriptive middleware names
- Document what context keys middleware uses/sets

**DON'T:**
- Put business logic in middleware
- Assume specific events will be in the chain
- Modify event behavior in breaking ways
- Create tight coupling between middleware layers

### Context Management

**DO:**
- Use consistent naming conventions for context keys
- Document expected context keys for each event
- Validate context state at chain boundaries
- Consider context immutability in functional implementations

**DON'T:**
- Store sensitive data in context longer than needed
- Use generic names like "data" or "result"
- Assume keys exist (always check/validate)
- Leak context between separate chain executions

### Error Handling

**DO:**
- Choose appropriate fault tolerance mode for use case
- Log errors with sufficient context
- Provide meaningful error messages
- Use typed errors when possible

**DON'T:**
- Catch and swallow errors silently
- Use exceptions for control flow
- Return success when operation actually failed
- Ignore partial failures in lenient mode

---

## Migration and Adoption

### Incremental Adoption

You don't need to rewrite everything at once:

**Phase 1: Identify a Workflow**
- Pick a sequential process in your codebase
- Something with multiple steps currently in one function

**Phase 2: Extract Events**
- Convert each step to an event
- Move shared state to context

**Phase 3: Add Chain Orchestration**
- Create chain and add events
- Keep original function as wrapper initially

**Phase 4: Add Middleware**
- Extract cross-cutting concerns
- Apply to chain instead of individual events

**Phase 5: Expand Usage**
- Apply pattern to similar workflows
- Build library of reusable events and middleware

### Refactoring Example

**Before:**
```
function process_order(order):
    validate_order(order)
    log("Order validated")
    
    totals = calculate_totals(order)
    log("Totals calculated")
    
    payment = process_payment(totals)
    log("Payment processed")
    
    send_confirmation(order, payment)
    log("Confirmation sent")
    
    return payment
```

**After:**
```
chain = new EventChain()
chain.use_middleware(new LoggingMiddleware())

chain.add_event(new ValidateOrderEvent())
chain.add_event(new CalculateTotalsEvent())
chain.add_event(new ProcessPaymentEvent())
chain.add_event(new SendConfirmationEvent())

context.set("order", order)
result = chain.execute()

return context.get("payment")
```

---

## Common Pitfalls and Solutions

### Pitfall 1: Overly Fine-Grained Events
**Problem:** Creating an event for every tiny operation
**Solution:** Events should represent meaningful steps in the workflow

### Pitfall 2: Context Bloat
**Problem:** Context becomes dumping ground for all data
**Solution:** Only store data that multiple events need

### Pitfall 3: Middleware Ordering Confusion
**Problem:** Not understanding LIFO execution order
**Solution:** Use the gift-wrapping mental model; test thoroughly

### Pitfall 4: Ignoring Error Handling
**Problem:** Not choosing appropriate fault tolerance mode
**Solution:** Think about error scenarios; test failure paths

### Pitfall 5: Tight Coupling
**Problem:** Events knowing about specific other events
**Solution:** Events should only communicate through context

### Pitfall 6: Missing Abstraction
**Problem:** Duplicating chain setup across similar workflows
**Solution:** Create chain builder functions or templates

---

## Performance Considerations

### Overhead Analysis

**Minimal Overhead:**
- Function calls for each middleware layer
- Context lookups (dictionary access)
- Result object creation

**Generally Negligible When:**
- Events do substantial work (database calls, API requests)
- Chain executes infrequently (request processing)
- Clarity and maintainability outweigh microseconds

**Optimize When:**
- High-frequency execution (thousands per second)
- Tight latency requirements (microseconds matter)
- Resource-constrained environments

### Optimization Strategies

**1. Context Pooling**
Reuse context objects instead of allocating new ones

**2. Compiled Pipelines**
Build middleware pipeline once, reuse for multiple executions

**3. Lazy Context Access**
Only fetch context values when actually needed

**4. Inline Critical Paths**
For hot paths, consider flattening some layers

**5. Async/Parallel Where Possible**
Use async events and await in appropriate places

---

## Future Directions and Variations

### Async/Await Support

Modern languages enable truly asynchronous chains:
```
async function execute():
    for event in events:
        result = await event.execute_async(context)
```

### Distributed EventChains

Extend pattern across services:
- Events run on different machines
- Context serialized and transmitted
- Saga pattern integration for compensation

### Visual Chain Builders

GUI tools for non-programmers:
- Drag-and-drop event ordering
- Visual middleware configuration
- Runtime chain modification

### Machine Learning Integration

Adaptive chains that learn:
- Optimize event ordering based on data
- Skip unnecessary events dynamically
- Predict likely failures and preemptively handle

---

## Conclusion

EventChains is a powerful, flexible design pattern that brings order to sequential workflows. By separating business logic (events) from infrastructure concerns (middleware) and orchestrating them through a shared context, it delivers:

- **Clarity** through procedural execution flow
- **Composability** through functional design principles  
- **Flexibility** through object-oriented abstractions
- **Maintainability** through separation of concerns
- **Testability** through isolated components
- **Reusability** through modular design

Whether you're building web APIs, game engines, data pipelines, or business workflows, EventChains adapts to your language, your paradigm, and your problem domain while maintaining a consistent, understandable conceptual model.

Start small, adopt incrementally, and let the pattern grow with your needs.

---

## Additional Resources

### Reference Implementations

- **C# (.NET):** https://github.com/RPDevJesco/EventChains-CS
- **Ruby:** https://github.com/RPDevJesco/EventChains-Ruby
- **Java:** (See example implementations in documents)
- **C:** (See example implementations in documents)

### Related Patterns

- Chain of Responsibility Pattern
- Pipeline Pattern
- Decorator Pattern
- Middleware Pattern
- Saga Pattern

### Further Reading

- "Design Patterns: Elements of Reusable Object-Oriented Software" by Gang of Four
- "Domain-Driven Design" by Eric Evans (for context-driven design)
- "Release It!" by Michael Nygard (for resilience patterns)

---

**Document Version:** 1.0  
**Last Updated:** October 2025  
**Author:** Based on EventChains implementation by RPDevJesco (Jesse Glover)
