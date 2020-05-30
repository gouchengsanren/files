#ifndef __BUTTON_H__
#define __BUTTON_H__

struct button_operations {
    int count;
    void (*init) (int which);
    int (*read) (int which);
};

void register_button_operations(struct button_operations *ops);
void unregister_button_operations(void);

#endif

