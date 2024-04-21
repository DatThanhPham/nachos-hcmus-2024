// pcb.h 
//  Process Control Block
//	Save all information to control the process
// All rights reserved.


#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"

// Process Control Block
class PCB
{
private:
    Semaphore* joinSem;         // semaphore cho quá trình join
    Semaphore* exitSem;         // semaphore cho quá trình exit
    Semaphore* mutex;          // semaphore cho quá trình truy xuất đọc quyền  

    int exitCode;		
    int numwait;                // số tiến trình đã join

    int	pid;

    char FileName[32];          // Ten cua tien trinh

    Thread* thread;             // Tien trinh cua chuong trinh
public:
    int parentID;               // ID cua tien trinh cha

    int	JoinStatus;
    
    PCB(int = 0);               // Contructor
    ~PCB();                     // Destructor

    int Exec(char*,int);        // Tao mot thread moi
    int GetID();                // Trả về ProcessID của tiến trình gọi thực hiện
    int GetNumWait();           // Trả về số lượng tiến trình chờ


    void JoinWait();            // 1. Tiến trình cha đợi tiến trình con kết thúc
                        
    void ExitWait();             // 4. Tiến trình con kết thúc

    void JoinRelease();         // 2. Báo cho tiến trình cha thực thi tiếp
    void ExitRelease();         // 3. Cho phép tiến trình con kết thúc

    void IncNumWait();          // Tăng số tiến trình chờ
    void DecNumWait();          // Giảm số tiến trình chờ

    void SetExitCode(int);      // Đặt exitcode của tiến trình
    int GetExitCode();          // Trả về exitcode

    void SetFileName(char*);    // Set ten tien trinh
    char* GetFileName();        // Tra ve ten tien trinh

};

void MyStartProcess(int pID);

#endif // PCB_H

