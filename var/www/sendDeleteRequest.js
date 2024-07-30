'using strict'

function fetchFiles() {
	fetch('/cgi-bin/list_files.cgi')
		.then(response => response.json())
		.then(files => {
			const fileList = document.getElementById('fileList');
			fileList.innerHTML = ''; // Clear the list before populating

			files.forEach(file => {
				const li = document.createElement('li');
				li.innerHTML = `
					<input type="checkbox" name="files" value="${file}">
					${file}
				`;
				fileList.appendChild(li);
			});
		})
		.catch(error => console.error('Error fetching file list:', error));
}

// Function to send DELETE request for selected files
function deleteSelectedFiles() {
	const selectedFiles = Array.from(document.querySelectorAll('input[name="files"]:checked'))
		.map(input => input.value);

	selectedFiles.forEach(file => {
		fetch(`/cgi-bin/delete_file.cgi?filename=${encodeURIComponent(file)}`, {
			method: 'DELETE',
			headers: {
				'Content-Type': 'application/json'
			}
		})
		.then(response => {
			if (response.ok) {
				alert(`File ${file} deleted successfully`);
				fetchFiles(); // Refresh file list after deletion
			} else {
				alert(`Error deleting file ${file}`);
			}
		})
		.catch(error => console.error('Error:', error));
	});
}

// Fetch file list on page load
fetchFiles();