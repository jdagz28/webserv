function copyToClipboard(text) {
    navigator.clipboard.writeText(text)
        .then(() => {
            alert("Copied to clipboard!");
        })
        .catch(err => {
            console.error("Failed to copy text: ", err);
            alert("Failed to copy text. Please try again.");
        });
}

document.addEventListener("DOMContentLoaded", () => {
    const copyButtons = document.querySelectorAll(".copy-btn");

    copyButtons.forEach(button => {
        const commandText = button.nextElementSibling.textContent;

        button.addEventListener("click", () => {
            copyToClipboard(commandText);
        });
    });
});
