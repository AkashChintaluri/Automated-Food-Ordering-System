#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    DINE_IN,
    TAKE_AWAY,
    HOME_DELIVERY
} OrderType;

typedef struct {
    int orderNumber;
    OrderType type;
    char details[100];
    int prep_time;
    int distance;
    int priority;
    int waitingTime;
} Order;

typedef struct Node {
    Order order;
    struct Node *next;
} Node;

Node* priorityQueue = NULL;

const char* available_items[] = {"Burger", "Pizza", "Fries", "Coke"};
const int prep_times[] = {10, 15, 5, 2};

int orderNumber = 0;

int get_prep_time(int itemNumber) {
    if (itemNumber >= 1 && itemNumber <= 4) {
        return prep_times[itemNumber - 1];
    }
    return -1;
}

const char* get_order_type_string(OrderType type) {
    switch(type) {
        case DINE_IN: return "Dine In";
        case TAKE_AWAY: return "Take Away";
        case HOME_DELIVERY: return "Home Delivery";
        default: return "Unknown";
    }
}

const char* get_item_name(int itemNumber) {
    if (itemNumber >= 1 && itemNumber <= 4) {
        return available_items[itemNumber - 1];
    }
    return "Unknown";
}

void insert_order(Order newOrder) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->order = newOrder;
    newNode->next = NULL;

    if (!priorityQueue) {
        priorityQueue = newNode;
        return;
    }

    if (newNode->order.priority < priorityQueue->order.priority) {
        newNode->next = priorityQueue;
        priorityQueue = newNode;
    } else {
        Node *current = priorityQueue;
        while (current->next && current->next->order.priority <= newNode->order.priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

Order pop_order() {
    if (!priorityQueue) {
        fprintf(stderr, "No orders in queue\n");
        exit(1);
    }

    Order topOrder = priorityQueue->order;
    Node *temp = priorityQueue;
    priorityQueue = priorityQueue->next;
    free(temp);
    return topOrder;
}

void show_menu() {
    printf("\nWelcome to the Restaurant!\n");
    printf("Choose an option:\n");
    printf("1. Dine In\n");
    printf("2. Take Away\n");
    printf("3. Home Delivery\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
}

void print_separator() {
    printf("\n----------------------------------------\n");
}

void clear_screen() {
    printf("\033[H\033[J");
}

int main() {
    int choice;

    while (1) {
        clear_screen();
        show_menu();
        scanf("%d", &choice);

        Order newOrder;
        newOrder.orderNumber = ++orderNumber;

        switch (choice) {
            case 1:
            case 2:
            case 3:
                printf("\nAvailable items:\n");
                for (int i = 1; i <= 4; i++) {
                    printf("%d. %s\n", i, available_items[i - 1]);
                }
                printf("Enter the food item number: ");
                int itemNumber;
                scanf("%d", &itemNumber);

                int prepTime = get_prep_time(itemNumber);
                if (prepTime == -1) {
                    printf("Invalid item selected. Please try again.\n");
                    continue;
                }
                newOrder.prep_time = prepTime;

                if (choice == 3) {
                    printf("Enter distance from restaurant (in km): ");
                    scanf("%d", &newOrder.distance);
                    newOrder.priority = 3000 + newOrder.distance;
                } else if (choice == 2) {
                    newOrder.priority = 2000 - orderNumber;
                } else {
                    newOrder.priority = 1000 - orderNumber;
                }

                newOrder.type = (OrderType)(choice - 1);
                strcpy(newOrder.details, get_item_name(itemNumber));
                insert_order(newOrder);
                break;

            case 4:
                clear_screen();
                int totalTime = 0;
                Order processedOrders[100];
                int processedIndex = 0;

                printf("Processing orders...\n");
                while (priorityQueue) {
                    Order currentOrder = pop_order();
                    currentOrder.waitingTime = totalTime; // Set waiting time to the total time passed
                    totalTime += currentOrder.prep_time;
                    processedOrders[processedIndex++] = currentOrder;

                    printf("Order No %d (%s): %s\n", currentOrder.orderNumber,
                           get_order_type_string(currentOrder.type),
                           currentOrder.details);
                    print_separator();
                }

                while (1) {
                    printf("Enter an order number to see its waiting time or 0 to exit: ");
                    int queryOrderNum;
                    scanf("%d", &queryOrderNum);

                    if (queryOrderNum == 0) {
                        return 0;
                    }

                    int found = 0;
                    for (int i = 0; i < processedIndex; i++) {
                        if (processedOrders[i].orderNumber == queryOrderNum) {
                            printf("Order No %d had a waiting time of %d minutes.\n", queryOrderNum, processedOrders[i].waitingTime);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("Order number not found in processed orders.\n");
                    }
                }
                break;

            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}

