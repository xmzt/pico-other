add_executable(selfprog selfprog.c mico_usb.c)
pico_add_extra_outputs(selfprog)
target_link_libraries(selfprog PRIVATE pico_stdlib hardware_resets hardware_irq)

# add url via pico_set_program_url
#example_auto_set_url(selfprog)
