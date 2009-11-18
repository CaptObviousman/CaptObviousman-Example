#ifndef CLERK_H
#define CLERK_H

void putBoxOnShelf();
box getBox(int box_num);
void* clerkThread(void* arg);

#endif
