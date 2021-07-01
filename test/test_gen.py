#By: Benjamin Herrera Navarro
#Mon, Jun 28, 1:39AM

import random
from random import randrange
from ctypes import *
import numpy as np

gen_single_fail_pass = 0
ebreak = 1
uart_base = 0x10000000

tests = {
#    #OP
#    "add": 5000,
#    "sub": 5000,
#    "sll": 5000,
#    "slt": 5000,
#    "sltu": 5000,
#    "xor": 5000,
#    "srl": 5000,
#    "sra": 5000,
#    "or": 5000,
#    "and": 5000,
#    #OP_IM
#    "slli": 5000,
    "addi": 5000,
#    "slti": 5000,
#    "sltiu": 5000,
#    "xori": 5000,
#    "srli": 5000,
#    "srai": 5000,
#    "ori": 5000,
#    "andi": 5000
}

#tests = {
#    #OP
#    "add": 10,
#    "sub": 10,
#    "sll": 10,
#    "slt": 10,
#    "sltu": 10,
#    "xor": 10,
#    "srl": 10,
#    "sra": 10,
#    "or": 10,
#    "and": 10,
#    #OP_IM
#    "slli": 10,
#    "addi": 10,
#    "slti": 10,
#    "sltiu": 10,
#    "xori": 10,
#    "srli": 10,
#    "srai": 10,
#    "ori": 10,
#    "andi": 10
#}

#    {"ADD", add_gen},
#    {"SUB", sub_gen},
#    {"SLL", sll_gen},
#    {"SLT", slt_gen},
#    {"SLTU", sltu_gen},
#    {"XOR", xor_gen},
#    {"SRL", srl_gen},
#    {"SRA", sra_gen},
#    {"OR", or_gen},
#    {"AND", and_gen}
#]

def gen_jump(ins_buffer, target):
    #Make x1 not zero
    ins_buffer.append(indent("mv x1, zero"))
    ins_buffer.append(indent("addi x1, x1, 1"))
    
    #Label for this error message function
    ins_buffer.append("{0}_halt_{1}:".format(ins, prefix))
    ins_buffer.append(indent("bne x1, zero, {0}".format(target)))

    return ins_buffer

def sign_extend(value, bits):
    sign_bit = 1 << (bits - 1)
    return (value & (sign_bit - 1)) - (value & sign_bit)

def gen_print(ins_buffer, string):

    ins_buffer.append(indent("print_msg_{0}:".format(string)))

    #Transform to upper case
    message = string

    for i in message:
        ins_buffer.append(indent("mv x2, zero"))
        ins_buffer.append(indent("addi x2, x2, '{0}'".format(i)))
        ins_buffer.append(indent("sb x2, 0(x1)"))

    ins_buffer.append(indent("mv x2, zero"))
    ins_buffer.append(indent("addi x2, x2, '\\n'"))
    ins_buffer.append(indent("sb x2, 0(x1)"))

    return ins_buffer

def gen_ins_op(op, rd, rs1, rs2):
    return "{0}, {1}, {2}, {3}".format(op, rd, rs1, rs)

#This function will check if the LUI and ADDI functions are working correctly
def check_requirements(ins_buffer):
    #Label for this test unit
    ins_buffer.append("requirement_check:")

    #Generate random register value
    rd = randrange(31)

    #generate value to be loaded
    loaded = randrange(0xfff)


    #Shift value to simulate load upper immidiate
    expected = loaded << 12

    #if rd is zero, then the expected value must be 0
    if rd == 0:
        expected = 0

    ins_buffer.append("lui x{0}, {1}".format(rd, loaded))

    #test if the value is correct
    #load value into register, use rs1 or next register is rd 
    #Try to use the register already used by rs1
    if((rs1 == rd) or (rs1 == 0 and rd != 0)):
        if rd == 31:
            rs1 = 1
        else:
            rs1 = rd + 1

    #Load expected into rs1
    #ins_buffer.append("")

    ins_buffer.append("bne x{0}, x{1}, check_requirements_lui_fail".format(rd, rs1));

    return ins_buffer

#The registers rs1 and rs2 will be modified by this funcfion
def gen_fail(ins_buffer, prefix, ins, expected, uart_base):
    #Generate error message function
    #Label for this error message function
    ins_buffer.append("{0}_fail_{1}:".format(ins, prefix))

    #Load uart base
    ins_buffer.append(indent("li x1, {0}".format(hex(uart_base))))

    #Transform to upper case
    message = ins.upper() + " {0} FAIL".format(prefix)

    for i in message:
        ins_buffer.append(indent("mv x2, zero"))
        ins_buffer.append(indent("addi x2, x2, '{}'".format(i)))
        ins_buffer.append(indent("sb x2, 0(x1)"))

    ins_buffer.append(indent("mv x2, zero"))
    ins_buffer.append(indent("addi x2, x2, '\\n'"))
    ins_buffer.append(indent("sb x2, 0(x1)"))
    
    ins_buffer.append(indent("ebreak"))
    #Make x1 not zero
    ins_buffer.append(indent("mv x1, zero"))
    ins_buffer.append(indent("addi x1, x1, 1"))
    
    #Label for this error message function
    ins_buffer.append("{0}_halt_{1}:".format(ins, prefix))
    ins_buffer.append(indent("bne x1, zero, {0}_halt_{1}".format(ins, prefix)))

    return ins_buffer

#The registers rs1 and rs2 will be modified by this funcfion
def gen_pass(ins_buffer, prefix, ins, expected, uart_base):
    #Generate pass message function
    #Label for this pass message function
    ins_buffer.append("{0}_pass_{1}:".format(ins, prefix))

    #Load uart base
    ins_buffer.append(indent("li x1, {0}".format(hex(uart_base))))

    #Transform to upper case
    message = ins.upper() + " {0} PASS".format(prefix)

    for i in message:
        ins_buffer.append(indent("mv x2, zero"))
        ins_buffer.append(indent("addi x2, x2, '{}'".format(i)))
        ins_buffer.append(indent("sb x2, 0(x1)"))

    ins_buffer.append(indent("mv x2, zero"))
    ins_buffer.append(indent("addi x2, x2, '\\n'"))
    ins_buffer.append(indent("sb x2, 0(x1)"))
    
    #Make x1 not zero
    ins_buffer.append(indent("mv x1, zero"))
    ins_buffer.append(indent("addi x1, x1, 1"))
    
    #jump to entry point
    ins_buffer.append(indent("bne x1, zero, {0}_{1}".format("entry", (prefix+1))))

    return ins_buffer

def indent(ins):
    return "    " + ins

def sra(x,n,m):
    if x & 2**(n-1) != 0:  # MSB is 1, i.e. x is negative
        filler = int('1'*m + '0'*(n-m),2)
        x = (x >> m) | filler  # fill in 0's with 1's
        return x
    else:
        return x >> m

#This only generated tests for OP alu operations
def gen_test_op(ntest, ins_op):
    rs1 = randrange(31)
    rs1d = randrange(0xffffffff)
    rs2 = randrange(31)
    rs2d = randrange(0xffffffff)
    rd = randrange(31)
    
    #Increment rs2 to some other value instead
    if rs1 == rs2:
        if rs1 == 31:
            rs2 = 1
        else:
            rs2 = rs1 + 1


    #set data to 0 if they use zero register
    if rs1 == 0:
        rs1d = 0

    if rs2 == 0:
        rs2d = 0

    switcher ={
        "add": lambda n1,n2: c_int32(n1+n2).value,
        "sub": lambda n1,n2: c_int32(n1-n2).value,
        "sll": lambda n1,n2: c_uint32(n1 << (n2 & 0b11111)).value,
        "slt": lambda n1,n2: c_uint32(c_int32(n1).value < c_int32(n2).value).value,
        "sltu": lambda n1,n2: np.uint32(np.uint32(n1) < np.uint32(n2)),
        "xor": lambda n1,n2: c_int32(n1 ^ n2).value,
        "srl": lambda n1,n2: c_uint32(n1 >> (n2 & 0b11111)).value,
        "sra": lambda n1,n2: sra(n1, 32, (n2 & 0b11111)),
        "or": lambda n1,n2: c_int32(n1 | n2).value,
        "and": lambda n1,n2: c_int32(n1 & n2).value
    }

    #Calculate expected value
    expected = switcher[ins_op](rs1d, rs2d)
    
    
    ins = []
    #setup tag
    ins.append("{1}_{0}:".format(ntest, ins_op))

    fmt = "{0} x{1}, {2}"
    #setup rs1
    ins.append(indent(fmt.format("li", rs1, hex(rs1d))))
    #setup rs2
    ins.append(indent(fmt.format("li", rs2, hex(rs2d))))
    
    #Calculate Value
    fmt = "{0} x{1}, x{2}, x{3}"
    ins.append(indent(fmt.format(ins_op, rd, rs1, rs2))) 

    #Test if the value is expected
    #if rsd is equal to rs1, change rs1 to something else
    if rs1 == rd:
        if rd == 31:
            rs1 = 1
        else:
            rs1 = rd + 1
    if rs1 == 0:
        if rd == 1:
            rs1 = 2
        else:
            rs1 = 1
    #load expected value or zero if rd is zero
    if rd == 0:
        ins.append(indent("li x{0}, 0x00000000").format(rs1))
    else:
        ins.append(indent("li x{0}, {1}".format(rs1, hex(expected))))
    if gen_single_fail_pass != 1:
        #Jump to fail, if not equal
        ins.append(indent("bne x{0}, x{1}, {2}_fail_{3}".format(rs1, rd, ins_op, ntest)))
        #Else jump to pass
        #Make not equal and jump
        #Make x1 not zero
        ins.append(indent("mv x1, zero"))
        ins.append(indent("addi x1, x1, 1"))
        ins.append(indent("bne x1, zero, {0}_pass_{1}".format(ins_op, ntest)))

        ins = gen_fail(ins, ntest, ins_op, expected, uart_base)
        ins = gen_pass(ins, ntest, ins_op, expected, uart_base)
    else:
        #Jump to fail, if not equal
        ins.append(indent("bne x{0}, x{1}, fail".format(rs1, rd, ins_op, ntest)))
        #Else jump to pass
        #Make not equal and jump
        #Make x1 not zero
        ins.append(indent("mv x1, zero"))
        ins.append(indent("addi x1, x1, 1"))
        #jump to entry point
        ins.append(indent("bne x1, zero, {0}_{1}".format("entry", (ntest+1))))

    return ins

#This only generated tests for OP_IM alu operations
def gen_test_op_im(ntest, ins_op):
    #Generate random register value
    rs1 = randrange(31)
    rs2 = randrange(31)
    rd = randrange(31)
    
    #Generate the data that will be used
    rs1d = randrange(0xffffffff)
    imm = sign_extend(randrange(-2048, 2047), 12)

    #set data to 0 if they use zero register
    if rs1 == 0:
        rs1d = 0

    switcher ={
        "addi": lambda n1,n2: c_int32(n1+n2).value,
        "slli": lambda n1,n2: c_uint32(n1 << (n2 & 0b11111)).value,
        "slti": lambda n1,n2: c_uint32(c_int32(n1).value < c_int32(n2).value).value,
        "sltiu": lambda n1,n2: np.uint32(np.uint32(n1) < np.uint32(n2)),
        "xori": lambda n1,n2: c_int32(n1 ^ n2).value,
        "srli": lambda n1,n2: c_uint32(n1 >> (n2 & 0b11111)).value,
        "srai": lambda n1,n2: sra(n1, 32, (n2 & 0b11111)),
        "ori": lambda n1,n2: c_int32(n1 | n2).value,
        "andi": lambda n1,n2: c_int32(n1 & n2).value
    }

    #Calculate expected value
    expected = switcher[ins_op](rs1d, imm)
    
    
    ins = []
    #setup tag
    ins.append("{1}_{0}:".format(ntest, ins_op))

    fmt = "{0} x{1}, {2}"
    #setup rs1
    ins.append(indent(fmt.format("li", rs1, hex(rs1d))))
    
    mask_by5 = ["slli", "srli", "srai"]

    #Calculate Value
    if ins_op in mask_by5:
        fmt = "{0} x{1}, x{2}, {3}"
        ins.append(indent(fmt.format(ins_op, rd, rs1, imm & 0b11111))) 

    else:
        fmt = "{0} x{1}, x{2}, {3}"
        ins.append(indent(fmt.format(ins_op, rd, rs1, imm))) 

    #Test if the value is expected
    #if rsd is equal to rs1, change rs1 to something else
    if rs1 == rd:
        if rd == 31:
            rs1 = 1
        else:
            rs1 = rd + 1
    if rs1 == 0:
        if rd == 1:
            rs1 = 2
        else:
            rs1 = 1
    #load expected value or zero if rd is zero
    if rd == 0:
        ins.append(indent("li x{0}, 0x00000000").format(rs1))
    else:
        ins.append(indent("li x{0}, {1}".format(rs1, hex(expected))))
    if gen_single_fail_pass != 1:
        #Jump to fail, if not equal
        ins.append(indent("bne x{0}, x{1}, {2}_fail_{3}".format(rs1, rd, ins_op, ntest)))
        #Else jump to pass
        #Make not equal and jump
        #Make x1 not zero
        ins.append(indent("mv x1, zero"))
        ins.append(indent("addi x1, x1, 1"))
        ins.append(indent("bne x1, zero, {0}_pass_{1}".format(ins_op, ntest)))

        ins = gen_fail(ins, ntest, ins_op, expected, uart_base)
        ins = gen_pass(ins, ntest, ins_op, expected, uart_base)
    else:
        #Jump to fail, if not equal
        ins.append(indent("bne x{0}, x{1}, fail".format(rs1, rd, ins_op, ntest)))
        #Else jump to pass
        #Make not equal and jump
        #Make x1 not zero
        ins.append(indent("mv x1, zero"))
        ins.append(indent("addi x1, x1, 1"))
        #jump to entry point
        ins.append(indent("bne x1, zero, {0}_{1}".format("entry", (ntest+1))))

    return ins
    #print(ins)


def gen_tests(tests):
    index = 0
    ins_buffer = []
    
    ins = {
            "add" : gen_test_op,
            "sub" : gen_test_op,
            "sll" : gen_test_op,
            "slt" : gen_test_op,
            "sltu" : gen_test_op,
            "xor" : gen_test_op,
            "srl" : gen_test_op,
            "sra" : gen_test_op,
            "or" : gen_test_op,
            "and" : gen_test_op,
            "addi" : gen_test_op_im,
            "slli" : gen_test_op_im,
            "slti" : gen_test_op_im,
            "sltiu" : gen_test_op_im,
            "xori" : gen_test_op_im,
            "srli" : gen_test_op_im,
            "srai" : gen_test_op_im,
            "ori" : gen_test_op_im,
            "andi" : gen_test_op_im
            }

    #Generate Entry point for the first test case
    ins_buffer.append(["_start:\n"])
    
    for x in tests:
        for i in range(tests[x]):
            if index != 0:
                ins_buffer.append(["entry_{0}:".format(index)])
            ins_buffer.append(ins[x](index, x))
            index = index + 1    
#

    if gen_single_fail_pass:
        #Generate error message function
        #Label for this error message function
        ins_buffer.append(["fail:"])

        #Load uart base
        ins_buffer.append([indent("li x1, {0}".format(hex(uart_base)))])

        #Transform to upper case
        message = "TEST FAIL"

        for i in message:
            ins_buffer.append([indent("mv x2, zero")])
            ins_buffer.append([indent("addi x2, x2, '{}'".format(i))])
            ins_buffer.append([indent("sb x2, 0(x1)")])

        ins_buffer.append([indent("mv x2, zero")])
        ins_buffer.append([indent("addi x2, x2, '\\n'")])
        ins_buffer.append([indent("sb x2, 0(x1)")])

        if ebreak:
            ins_buffer.append([indent("ebreak")])
        else:
            #Make x1 not zero
            ins_buffer.append([indent("mv x1, zero")])
            ins_buffer.append([indent("addi x1, x1, 1")])

            #Label for this error message function
            ins_buffer.append(["fail_halt:"])
            ins_buffer.append([indent("bne x1, zero, fail_halt")])

    ins_buffer.append(["entry_{0}:".format(index)])
    #Transform to upper case
    message = "TESTS DONE"

    ins_buffer.append([indent("li x1, {0}".format(hex(uart_base)))])
    for i in message:
        ins_buffer.append([indent("mv x2, zero")])
        ins_buffer.append([indent("addi x2, x2, '{}'".format(i))])
        ins_buffer.append([indent("sb x2, 0(x1)")])

    ins_buffer.append([indent("mv x2, zero")])
    ins_buffer.append([indent("addi x2, x2, '\\n'")])
    ins_buffer.append([indent("sb x2, 0(x1)")])
    
    #Make x1 not zero
    ins_buffer.append([indent("mv x1, zero")])
    ins_buffer.append([indent("addi x1, x1, 1")])
    
    #halt loop
    ins_buffer.append(["done_halt:"])
    ins_buffer.append([indent("bne x1, zero, done_halt")])

    file_data = ""
    #test that the 
    for x in ins_buffer:
        for y in x:
            file_data =  file_data + "{0}\n".format(y)

    f = open("rv32i_gen.s", "w")
    f.write(file_data)
    f.close()
    #n_tests tests.get[ins[index].lower()]
#
    #add_gen(23)

gen_tests(tests)

#def tester():
#
#
#def add(s1, s2, operator):
#    switcher ={
#        '+': lambda n1,n2: n1+n2,
#        '-': lambda n1,n2: n1-n2,
#        '*': lambda n1,n2: n1*n2,
#        '/': lambda n1,n2: n1/n2,
#    }

#print(OP)
#
#switched