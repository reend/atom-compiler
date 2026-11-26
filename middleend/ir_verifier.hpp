#pragma once

#include "ir.hpp"

class IRVerifier
{
public:
    void verify(const IRModule& module) const;

private:
    void verifyFunction(const IRFunction& function) const;

    void verifyBlock(const IRFunction& function,
                     const IRBasicBlock& block) const;

    bool isTerminator(IROpcode opcode) const;
};