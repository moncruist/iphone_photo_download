#include "context.h"


Context::Context() {
    context = gp_context_new();
}

Context::Context(GPContext* context) : context(context) {
    if (this->context) {
        gp_context_ref(this->context);
    }
}

Context::~Context() {
    if (context != nullptr) {
        gp_context_unref(context);
    }
}

Context::Context(const Context& other) : context(other.context) {
    gp_context_ref(context);
}

Context::Context(Context&& other) : context(other.context) {
    other.context = nullptr;
}

Context& Context::operator=(const Context& other) {
    context = other.context;
    gp_context_ref(context);
    return *this;
}

Context& Context::operator=(Context&& other) {
    context = other.context;
    other.context = nullptr;
    return *this;
}

GPContext* Context::get_context() const {
    return context;
}
