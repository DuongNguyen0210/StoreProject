# Update include paths in all source files
$files = Get-ChildItem -Recurse -Include *.cpp, *.h

foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw -Encoding UTF8
    $original = $content
    
    # Replace in order - most specific first
    $content = $content -replace '#include "Product\.h"', '#include "models/Product.h"'
    $content = $content -replace '#include "Food\.h"', '#include "models/Food.h"'
    $content = $content -replace '#include "Beverage\.h"', '#include "models/Beverage.h"'
    $content = $content -replace '#include "HouseholdItem\.h"', '#include "models/HouseholdItem.h"'
    $content = $content -replace '#include "Customer\.h"', '#include "models/Customer.h"'
    $content = $content -replace '#include "User\.h"', '#include "models/User.h"'
    $content = $content -replace '#include "Manager\.h"', '#include "models/Manager.h"'
    $content = $content -replace '#include "Cashier\.h"', '#include "models/Cashier.h"'
    $content = $content -replace '#include "Bill\.h"', '#include "models/Bill.h"'
    $content = $content -replace '#include "BillItem\.h"', '#include "models/BillItem.h"'
    $content = $content -replace '#include "Payment\.h"', '#include "models/Payment.h"'
    $content = $content -replace '#include "Store\.h"', '#include "core/Store.h"'
    $content = $content -replace '#include "(StorePersistence|storepersistence)\.h"', '#include "core/storepersistence.h"'
    $content = $content -replace '#include "(HashTable|hashtable)\.h"', '#include "core/HashTable.h"'
    $content = $content -replace '#include "mainwindow\.h"', '#include "ui/mainwindow.h"'
    $content = $content -replace '#include "ThongKe\.h"', '#include "ui/ThongKe.h"'
    $content = $content -replace '#include "AddCustomerToStore\.h"', '#include "dialogs/AddCustomerToStore.h"'
    $content = $content -replace '#include "AddProductToStore\.h"', '#include "dialogs/AddProductToStore.h"'
    $content = $content -replace '#include "(EditProductDialog|editproductdialog)\.h"', '#include "dialogs/EditProductDialog.h"'
    $content = $content -replace '#include "billdetaildialog\.h"', '#include "dialogs/billdetaildialog.h"'
    $content = $content -replace '#include "StockProductDialog\.h"', '#include "dialogs/StockProductDialog.h"'
    $content = $content -replace '#include "logindialog\.h"', '#include "dialogs/logindialog.h"'
    $content = $content -replace '#include "(ManageInventory|manageinventory)\.h"', '#include "dialogs/ManageInventory.h"'
    
    if ($content -ne $original) {
        Set-Content $file.FullName -Value $content -NoNewline -Encoding UTF8
        Write-Host "Updated: $($file.Name)"
    }
}
Write-Host "All include paths updated successfully!"
